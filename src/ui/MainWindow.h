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

    void renderAudioInfo();

    // Main GUI functions for HelloImGui
    void showGui();
    void showMenus();
    void showStatus();
    
    // File operations
    void openAudioFile();
    
private:
    struct Marker
    {
        std::string name;
        float timeSeconds = 0.0f;
    };

    struct ApplicationState
    {
        std::vector<Marker> markers;
        std::string soundFilePath;
        float playPosition = 0.0f;
    };

    void renderAudioControls();

    void seekToPreviousMarker();

    void seekToNextMarker();

    void renderMarkerControls();
    void renderWaveformArea();
    void updateWaveformData();
    int currentMarkerIndex() const;
    void sortMarkers();

    // Application state
    AudioEngine m_audioEngine;
    WaveformRenderer m_waveformRenderer;
    ApplicationState m_appState;
    bool m_waveformDirty = false;
};