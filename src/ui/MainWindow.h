#pragma once
#include "audio/AudioEngine.h"
#include "ui/WaveformRenderer.h"
#include <string>

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();
    
    // Main GUI functions for HelloImGui
    void showGui();
    void showMenus();
    void showStatus();
    
    // File operations
    void openAudioFile();
    
private:
    void renderAudioControls();
    void renderWaveformArea();
    void updateWaveformData();

    // Application state
    AudioEngine m_audioEngine;
    WaveformRenderer m_waveformRenderer;
    bool m_waveformDirty = false;
    bool m_showDemo = false;
    bool m_showMetrics = false;
};