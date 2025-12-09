#pragma once
#include "audio/AudioEngine.h"
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
    
    // Application state
    AudioEngine m_audioEngine;
    bool m_showDemo = false;
    bool m_showMetrics = false;
};