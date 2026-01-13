#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <RtAudio.h>
#include <SoundTouch.h>

class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();
    
    // Basic playback control
    bool initialize();
    void shutdown();
    
    // Audio file handling
    bool loadAudioFile(const char* filePath);
    void unloadAudio();
    bool hasAudio() const;
    std::string loadedFilePath() const;
    bool streamReady() const;

    // Playback control
    void play();
    void pause();
    void stop();
    void seek(float timeSeconds);
    void seekBy(float delay);
    bool isPlaying() const;
    bool isStreamRunning() const;
    bool isPlaybackFinished() const;

    // Tempo control
    void setTempoMultiplier(float multiplier);
    float getTempoMultiplier() const;
    bool isTempoProcessing() const;
    float getTempoProcessingProgress() const;

    // Getters
    float getDuration() const;
    float getCurrentTime() const;
    uint32_t getSampleRate() const;
    uint32_t getChannelCount() const;
    uint64_t getFrameCount() const;
    const std::vector<float>& getAudioData() const;

private:
    bool loadWavFile(const char* filePath);
    bool loadMp3File(const char* filePath);
    void resetState();
    bool ensureStreamReadyLocked();
    bool openStreamLocked();
    void closeStreamLocked();
    int processAudio(float* output, unsigned int frames, RtAudioStreamStatus status);
    static int audioCallback(void* outputBuffer,
                             void* inputBuffer,
                             unsigned int nBufferFrames,
                             double streamTime,
                             RtAudioStreamStatus status,
                             void* userData);

    void reprocessAudioWithTempo(float multiplier);

    bool m_initialized = false;
    std::atomic<bool> m_playing{false};
    bool m_hasAudio = false;
    std::atomic<bool> m_endOfStream{false};
    std::atomic<float> m_currentTime{0.0f};
    std::atomic<float> m_tempoMultiplier{1.0f};
    float m_activeTempoMultiplier = 1.0f;  // Currently applied tempo
    std::atomic<bool> m_tempoProcessingInProgress{false};
    std::atomic<float> m_tempoProcessingProgress{0.0f};
    float m_duration = 0.0f;  // Always original duration
    uint32_t m_sampleRate = 0;
    uint32_t m_channelCount = 0;
    uint64_t m_frameCount = 0;  // Always original frame count
    uint64_t m_processedFrameCount = 0;  // Frame count of processed buffer
    std::atomic<uint64_t> m_playbackFrameIndex{0};  // Index in processed buffer
    std::vector<float> m_originalAudioBuffer;  // Original audio data
    std::vector<float> m_processedAudioBuffer; // Tempo-adjusted audio
    std::string m_loadedFilePath;


    std::unique_ptr<RtAudio> m_rtaudio;
    RtAudio::StreamParameters m_streamParams{};
    RtAudio::StreamOptions m_streamOptions{};
    unsigned int m_bufferFrames = 512;
    uint32_t m_streamSampleRate = 0;
    uint32_t m_streamChannels = 0;
    bool m_streamOpen = false;
    bool m_streamRunning = false;
    int m_defaultDeviceId = -1;
    std::mutex m_streamMutex;
};