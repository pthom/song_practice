#pragma once
#include "audio/AudioEngine.h"
#include "ui/WaveformRenderer.h"
#include <string>
#include <vector>

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
    void renderMarkerControls();
    void renderWaveformArea();
    void updateWaveformData();
    int currentMarkerIndex() const;
    void sortMarkers();

    struct Marker
    {
        std::string name;
        float timeSeconds = 0.0f;
    };

    // Application state
    AudioEngine m_audioEngine;
    WaveformRenderer m_waveformRenderer;
    std::vector<Marker> m_markers;
    bool m_waveformDirty = false;
    bool m_showDemo = false;
    bool m_showMetrics = false;
};