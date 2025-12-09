#pragma once

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
    
    // Playback control
    void play();
    void pause();
    void stop();
    bool isPlaying() const;
    
    // Getters
    float getDuration() const;
    float getCurrentTime() const;
    
private:
    bool m_initialized = false;
    bool m_playing = false;
    float m_duration = 0.0f;
    float m_currentTime = 0.0f;
};