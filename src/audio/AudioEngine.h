#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
    
    // Playback control
    void play();
    void pause();
    void stop();
    bool isPlaying() const;
    
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

    bool m_initialized = false;
    bool m_playing = false;
    bool m_hasAudio = false;
    float m_duration = 0.0f;
    float m_currentTime = 0.0f;
    uint32_t m_sampleRate = 0;
    uint32_t m_channelCount = 0;
    uint64_t m_frameCount = 0;
    std::vector<float> m_audioBuffer;
    std::string m_loadedFilePath;
};