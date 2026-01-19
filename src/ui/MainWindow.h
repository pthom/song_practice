#pragma once
#include "audio/AudioEngine.h"
#include "ui/WaveformRenderer.h"
#include "core/SettingsManager.h"
#include <string>
#include <vector>

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
    float tempoMultiplier = 1.0f;  // 1.0 = normal speed, 0.5 = half speed, 2.0 = double speed
};

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();

    // Settings management
    void loadSettings();
    void saveSettings();

    // App-level user prefs (recent settings list)
    void loadUserPrefs();
    void saveUserPrefs();

    // New session: load new audio file and clear markers/state if successful
    void newSessionWithFile(const std::string& filePath);

    void renderAudioInfo();

    // Main GUI functions for HelloImGui
    void showGui();
    void showMenus();
    void showStatus();
    
    // File operations
    void openAudioFile();

    // Track settings operations
    void loadTrackSettings();
    void saveTrackSettings();

private:

    void renderAudioControls();
    void renderTempoControls();
    void renderMarkerControls();
    void renderWaveformArea();
    void updateWaveformData();
    void handleKeyboardShortcuts();
    int currentMarkerIndex() const;
    void sortMarkers();

    bool loadTrackSettingsFromPath(const std::string& settingsPath);
    void addRecentSettingsPath(const std::string& settingsPath);

    void seekToPreviousMarker();

    void seekToNextMarker();

    // Application state
    AudioEngine m_audioEngine;
    WaveformRenderer m_waveformRenderer;
    ApplicationState m_appState;
    SettingsManager m_settingsManager;
    bool m_waveformDirty = false;
    bool m_wasTempoProcessing = false;
    float m_pendingTempoMultiplier = 1.0f;  // Tempo value in slider (not yet applied)
    std::vector<std::string> m_recentTrackSettings;
};