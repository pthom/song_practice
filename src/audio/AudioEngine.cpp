#include "AudioEngine.h"
#include "core/Utils.h"
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
        
    m_initialized = true;
    return true;
}

void AudioEngine::shutdown()
{
    if (!m_initialized)
        return;
        
    stop();
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
    m_currentTime = 0.0f;
    m_duration = (m_sampleRate > 0) ? static_cast<float>(m_frameCount) / static_cast<float>(m_sampleRate) : 0.0f;

    std::cout << "AudioEngine: Loaded file " << path
              << " (" << m_channelCount << " channels, "
              << m_sampleRate << " Hz, "
              << m_frameCount << " frames)" << std::endl;

    return true;
}

void AudioEngine::unloadAudio()
{
    stop();
    resetState();
}

void AudioEngine::play()
{
    if (!m_initialized || !m_hasAudio)
        return;
        
    m_playing = true;
}

void AudioEngine::pause()
{
    m_playing = false;
}

void AudioEngine::stop()
{
    m_playing = false;
    m_currentTime = 0.0f;
}

bool AudioEngine::isPlaying() const
{
    return m_playing;
}

bool AudioEngine::hasAudio() const
{
    return m_hasAudio;
}

std::string AudioEngine::loadedFilePath() const
{
    return m_loadedFilePath;
}

float AudioEngine::getDuration() const
{
    return m_duration;
}

float AudioEngine::getCurrentTime() const
{
    return m_currentTime;
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
    m_currentTime = 0.0f;
    m_hasAudio = false;
    m_loadedFilePath.clear();
}