#include "AudioEngine.h"
#include "core/Utils.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialize()
{
    if (m_initialized)
        return true;

    try
    {
        m_rtaudio = std::make_unique<RtAudio>(RtAudio::Api::UNSPECIFIED);
        if (m_rtaudio->getDeviceCount() == 0)
        {
            std::cerr << "AudioEngine: No audio output devices available" << std::endl;
            return false;
        }

        m_defaultDeviceId = static_cast<int>(m_rtaudio->getDefaultOutputDevice());
        if (m_defaultDeviceId < 0)
        {
            std::cerr << "AudioEngine: Failed to query default audio device" << std::endl;
            return false;
        }

        m_streamParams.deviceId = static_cast<unsigned int>(m_defaultDeviceId);
        m_streamParams.nChannels = 2;
        m_streamParams.firstChannel = 0;

        m_streamOptions.flags = 0;  // use interleaved buffers (default)
        m_streamOptions.streamName = "SongPractice";

        m_initialized = true;
        return true;
    }
    catch (...)
    {
        std::cerr << "AudioEngine: RtAudio initialization error" << std::endl;
        return false;
    }
}

void AudioEngine::shutdown()
{
    if (!m_initialized)
        return;

    stop();

    {
        std::lock_guard<std::mutex> lock(m_streamMutex);
        closeStreamLocked();
        m_rtaudio.reset();
    }

    unloadAudio();
    m_initialized = false;
}

bool AudioEngine::loadAudioFile(const char* filePath)
{
    if (filePath == nullptr)
        return false;

    std::string path(filePath);
    if (path.empty())
        return false;

    unloadAudio();

    std::string extension = Utils::getFileExtension(path);
    bool loaded = false;

    if (extension == "wav")
    {
        loaded = loadWavFile(path.c_str());
    }
    else if (extension == "mp3")
    {
        loaded = loadMp3File(path.c_str());
    }
    else
    {
        // Attempt to load using both handlers in case extension is missing or unusual
        loaded = loadWavFile(path.c_str());
        if (!loaded)
            loaded = loadMp3File(path.c_str());
    }

    if (!loaded)
    {
        resetState();
        std::cerr << "AudioEngine: Failed to load audio file " << path << std::endl;
        return false;
    }

    m_loadedFilePath = path;
    m_hasAudio = true;
    m_playbackFrameIndex.store(0);
    m_currentTime.store(0.0f);
    m_endOfStream.store(false);
    m_duration = (m_sampleRate > 0) ? static_cast<float>(m_frameCount) / static_cast<float>(m_sampleRate) : 0.0f;

    // Re-open stream for this audio format
    {
        std::lock_guard<std::mutex> lock(m_streamMutex);
        closeStreamLocked();
        m_streamSampleRate = m_sampleRate;
        m_streamChannels = m_channelCount;
        if (!openStreamLocked())
        {
            std::cerr << "AudioEngine: Failed to open RtAudio stream" << std::endl;
            m_hasAudio = false;
            resetState();
            return false;
        }
    }

    std::cout << "AudioEngine: Loaded file " << path
              << " (" << m_channelCount << " channels, "
              << m_sampleRate << " Hz, "
              << m_frameCount << " frames)" << std::endl;

    return true;
}

void AudioEngine::unloadAudio()
{
    stop();
    std::lock_guard<std::mutex> lock(m_streamMutex);
    closeStreamLocked();
    resetState();
}

void AudioEngine::play()
{
    if (!m_initialized || !m_hasAudio)
        return;

    std::lock_guard<std::mutex> lock(m_streamMutex);
    if (!ensureStreamReadyLocked())
        return;

    if (!m_streamRunning)
    {
        try
        {
            RtAudioErrorType result = m_rtaudio->startStream();
            if (result != RTAUDIO_NO_ERROR)
            {
                std::cerr << "AudioEngine: Failed to start stream - " << m_rtaudio->getErrorText() << std::endl;
                return;
            }
            m_streamRunning = true;
        }
        catch (...)
        {
            std::cerr << "AudioEngine: Failed to start stream" << std::endl;
            return;
        }
    }

    m_playing.store(true);
    m_endOfStream.store(false);
}

void AudioEngine::pause()
{
    m_playing.store(false);
}

void AudioEngine::stop()
{
    m_playing.store(false);
    m_playbackFrameIndex.store(0);
    m_currentTime.store(0.0f);
    m_endOfStream.store(false);
}

bool AudioEngine::isPlaying() const
{
    return m_playing.load();
}

bool AudioEngine::hasAudio() const
{
    return m_hasAudio;
}

std::string AudioEngine::loadedFilePath() const
{
    return m_loadedFilePath;
}

bool AudioEngine::isStreamRunning() const
{
    return m_streamRunning;
}

bool AudioEngine::isPlaybackFinished() const
{
    return m_endOfStream.load();
}

bool AudioEngine::streamReady() const
{
    return m_streamOpen;
}

float AudioEngine::getDuration() const
{
    return m_duration;
}

float AudioEngine::getCurrentTime() const
{
    return m_currentTime.load();
}

uint32_t AudioEngine::getSampleRate() const
{
    return m_sampleRate;
}

uint32_t AudioEngine::getChannelCount() const
{
    return m_channelCount;
}

uint64_t AudioEngine::getFrameCount() const
{
    return m_frameCount;
}

const std::vector<float>& AudioEngine::getAudioData() const
{
    return m_audioBuffer;
}

bool AudioEngine::loadWavFile(const char* filePath)
{
    drwav wav;
    if (!drwav_init_file(&wav, filePath, nullptr))
        return false;

    const drwav_uint64 totalFrames = wav.totalPCMFrameCount;
    const uint32_t channels = wav.channels;
    const uint32_t sampleRate = wav.sampleRate;

    if (totalFrames == 0 || channels == 0 || sampleRate == 0)
    {
        drwav_uninit(&wav);
        return false;
    }

    std::vector<float> buffer;
    buffer.resize(static_cast<size_t>(totalFrames) * channels);
    drwav_uint64 framesRead = drwav_read_pcm_frames_f32(&wav, totalFrames, buffer.data());
    drwav_uninit(&wav);

    if (framesRead == 0)
        return false;

    buffer.resize(static_cast<size_t>(framesRead) * channels);

    m_audioBuffer = std::move(buffer);
    m_channelCount = channels;
    m_sampleRate = sampleRate;
    m_frameCount = framesRead;

    return true;
}

bool AudioEngine::loadMp3File(const char* filePath)
{
    drmp3 mp3;
    if (!drmp3_init_file(&mp3, filePath, nullptr))
        return false;

    const drmp3_uint64 frameCount = drmp3_get_pcm_frame_count(&mp3);
    const uint32_t channels = mp3.channels;
    const uint32_t sampleRate = mp3.sampleRate;

    if (frameCount == 0 || channels == 0 || sampleRate == 0)
    {
        drmp3_uninit(&mp3);
        return false;
    }

    std::vector<float> buffer;
    buffer.resize(static_cast<size_t>(frameCount) * channels);
    drmp3_uint64 framesRead = drmp3_read_pcm_frames_f32(&mp3, frameCount, buffer.data());
    drmp3_uninit(&mp3);

    if (framesRead == 0)
        return false;

    buffer.resize(static_cast<size_t>(framesRead) * channels);

    m_audioBuffer = std::move(buffer);
    m_channelCount = channels;
    m_sampleRate = sampleRate;
    m_frameCount = framesRead;

    return true;
}

void AudioEngine::resetState()
{
    m_audioBuffer.clear();
    m_audioBuffer.shrink_to_fit();
    m_channelCount = 0;
    m_sampleRate = 0;
    m_frameCount = 0;
    m_duration = 0.0f;
    m_currentTime.store(0.0f);
    m_hasAudio = false;
    m_loadedFilePath.clear();
    m_playbackFrameIndex.store(0);
}

bool AudioEngine::ensureStreamReadyLocked()
{
    if (!m_streamOpen)
    {
        m_streamSampleRate = m_sampleRate;
        m_streamChannels = m_channelCount;
        if (!openStreamLocked())
            return false;
    }
    return true;
}

bool AudioEngine::openStreamLocked()
{
    if (!m_rtaudio)
        return false;

    if (m_streamOpen)
        return true;

    if (!m_hasAudio || m_streamChannels == 0 || m_streamSampleRate == 0)
        return false;

    m_streamParams.nChannels = m_streamChannels;

    try
    {
        RtAudioErrorType result = m_rtaudio->openStream(&m_streamParams,
                               nullptr,
                               RTAUDIO_FLOAT32,
                               m_streamSampleRate,
                               &m_bufferFrames,
                               &AudioEngine::audioCallback,
                               this,
                               &m_streamOptions);
        if (result != RTAUDIO_NO_ERROR)
        {
            std::cerr << "AudioEngine: Failed to open RtAudio stream - " << m_rtaudio->getErrorText() << std::endl;
            return false;
        }
        m_streamOpen = true;
        m_streamRunning = false;
        return true;
    }
    catch (...)
    {
        std::cerr << "AudioEngine: Failed to open RtAudio stream" << std::endl;
        m_streamOpen = false;
        return false;
    }
}

void AudioEngine::closeStreamLocked()
{
    if (!m_rtaudio || !m_streamOpen)
        return;

    try
    {
        if (m_rtaudio->isStreamRunning())
        {
            RtAudioErrorType result = m_rtaudio->stopStream();
            if (result != RTAUDIO_NO_ERROR)
            {
                std::cerr << "AudioEngine: Failed to stop RtAudio stream - " << m_rtaudio->getErrorText() << std::endl;
            }
            m_streamRunning = false;
        }
        if (m_rtaudio->isStreamOpen())
            m_rtaudio->closeStream();
    }
    catch (...)
    {
        std::cerr << "AudioEngine: Failed to close RtAudio stream" << std::endl;
    }

    m_streamOpen = false;
    m_streamRunning = false;
}

int AudioEngine::processAudio(float* output, unsigned int frames, RtAudioStreamStatus status)
{
    if (status != 0)
    {
        std::cerr << "AudioEngine: Stream underflow/overflow detected" << std::endl;
    }

    if (!m_playing.load() || !m_hasAudio)
    {
        std::fill(output, output + frames * m_streamChannels, 0.0f);
        return 0;
    }

    const uint64_t currentIndex = m_playbackFrameIndex.load();
    const uint64_t framesRemaining = (currentIndex < m_frameCount) ? (m_frameCount - currentIndex) : 0;
    const unsigned int framesToCopy = static_cast<unsigned int>(std::min<uint64_t>(frames, framesRemaining));

    const float* source = m_audioBuffer.data() + (currentIndex * m_streamChannels);
    std::copy(source, source + framesToCopy * m_streamChannels, output);

    if (framesToCopy < frames)
    {
        std::fill(output + framesToCopy * m_streamChannels, output + frames * m_streamChannels, 0.0f);
        m_playing.store(false);
        m_endOfStream.store(true);
    }

    m_playbackFrameIndex.store(currentIndex + framesToCopy);
    const float time = static_cast<float>(m_playbackFrameIndex.load()) / static_cast<float>(m_sampleRate);
    m_currentTime.store(time);

    return 0;
}

int AudioEngine::audioCallback(void* outputBuffer,
                               void* /*inputBuffer*/,
                               unsigned int nBufferFrames,
                               double /*streamTime*/,
                               RtAudioStreamStatus status,
                               void* userData)
{
    AudioEngine* engine = static_cast<AudioEngine*>(userData);
    if (engine == nullptr || outputBuffer == nullptr)
        return 0;

    return engine->processAudio(static_cast<float*>(outputBuffer), nBufferFrames, status);
}