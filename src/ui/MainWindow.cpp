#include "MainWindow.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "implot/implot.h"
#include "hello_imgui/hello_imgui.h"
#include "core/Utils.h"
#include "portable_file_dialogs/portable_file_dialogs.h"
#include "hello_imgui/icons_font_awesome_6.h"
#include <algorithm>
#include <filesystem>

namespace
{
    constexpr float kSeekStep = 1.0f;
    constexpr ImVec2 kTransportButtonSize = ImVec2(40.0f, 40.0f);
    constexpr float kTransportSpacing = 14.0f;
}


bool showControlButton(const char* icon, const char* tooltip, bool autorepeat = false)
{
    bool pressed = false;
    ImGui::PushID(icon);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.35f));
    if (autorepeat)
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
    pressed = ImGui::Button(icon, kTransportButtonSize);
    if (autorepeat)
        ImGui::PopItemFlag();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) && tooltip)
        ImGui::SetTooltip("%s", tooltip);
    ImGui::PopStyleColor();
    ImGui::PopID();
    return pressed;
}

MainWindow::MainWindow()
{
    m_audioEngine.initialize();
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    m_audioEngine.shutdown();
}

void MainWindow::loadSettings()
{
    if (m_settingsManager.loadGlobalSettings(m_appState))
    {
        // If we loaded a sound file path, try to load it
        if (!m_appState.soundFilePath.empty())
        {
            if (m_audioEngine.loadAudioFile(m_appState.soundFilePath.c_str()))
            {
                m_waveformDirty = true;
                // Seek to the saved play position
                if (m_appState.playPosition > 0.0f)
                {
                    m_audioEngine.seek(m_appState.playPosition);
                }
            }
            else
            {
                // File couldn't be loaded, clear the path
                m_appState.soundFilePath.clear();
                m_appState.playPosition = 0.0f;
                m_appState.markers.clear();
            }
        }
    }
}

void MainWindow::saveSettings()
{
    // Update current play position before saving
    if (m_audioEngine.hasAudio())
    {
        m_appState.playPosition = m_audioEngine.getCurrentTime();
    }

    m_settingsManager.saveGlobalSettings(m_appState);
}

void MainWindow::loadTrackSettings()
{
    // Use portable-file-dialogs for settings file selection
    auto selection = pfd::open_file("Load Track Settings",
                                   "",
                                   {"SongPractice Settings", "*.songpractice.json",
                                    "JSON Files", "*.json",
                                    "All Files", "*"},
                                   pfd::opt::none).result();

    if (!selection.empty())
    {
        const std::string settingsPath = selection[0];
        ApplicationState tempState;

        if (m_settingsManager.loadTrackSettings(settingsPath, tempState))
        {
            // Successfully loaded settings
            m_appState = tempState;

            // Try to load the audio file if specified
            if (!m_appState.soundFilePath.empty())
            {
                if (m_audioEngine.loadAudioFile(m_appState.soundFilePath.c_str()))
                {
                    m_waveformDirty = true;
                    // Seek to the saved play position
                    if (m_appState.playPosition > 0.0f)
                    {
                        m_audioEngine.seek(m_appState.playPosition);
                    }
                    HelloImGui::Log(HelloImGui::LogLevel::Info, "Loaded track settings and audio file: %s",
                                  Utils::getFileName(m_appState.soundFilePath).c_str());
                }
                else
                {
                    HelloImGui::Log(HelloImGui::LogLevel::Warning,
                                  "Loaded track settings but couldn't load audio file: %s",
                                  Utils::getFileName(m_appState.soundFilePath).c_str());
                }
            }
            else
            {
                HelloImGui::Log(HelloImGui::LogLevel::Info, "Loaded track settings: %s",
                              Utils::getFileName(settingsPath).c_str());
            }
        }
        else
        {
            HelloImGui::Log(HelloImGui::LogLevel::Error, "Failed to load track settings: %s",
                          Utils::getFileName(settingsPath).c_str());
        }
    }
}

void MainWindow::saveTrackSettings()
{
    // Update current play position before saving
    if (m_audioEngine.hasAudio())
    {
        m_appState.playPosition = m_audioEngine.getCurrentTime();
    }

    // Default filename based on current audio file
    std::string defaultName = "track-settings";
    if (!m_appState.soundFilePath.empty())
    {
        std::filesystem::path audioPath(m_appState.soundFilePath);
        defaultName = audioPath.stem().string() + "-settings";
    }
    defaultName += ".songpractice.json";

    // Use portable-file-dialogs for save location selection
    auto result = pfd::save_file("Save Track Settings",
                                defaultName,
                                {"SongPractice Settings", "*.songpractice.json",
                                 "JSON Files", "*.json",
                                 "All Files", "*"}).result();

    if (!result.empty())
    {
        std::string settingsPath = result;

        // Ensure the file has the correct extension
        if (!Utils::stringEndsWith(settingsPath, ".songpractice.json"))
        {
            settingsPath += ".songpractice.json";
        }

        if (m_settingsManager.saveTrackSettings(settingsPath, m_appState))
        {
            HelloImGui::Log(HelloImGui::LogLevel::Info, "Saved track settings: %s",
                          Utils::getFileName(settingsPath).c_str());
        }
        else
        {
            HelloImGui::Log(HelloImGui::LogLevel::Error, "Failed to save track settings: %s",
                          Utils::getFileName(settingsPath).c_str());
        }
    }
}

void MainWindow::renderAudioInfo()
{
    if (m_audioEngine.hasAudio())
    {
        const std::string fileName = Utils::getFileName(m_audioEngine.loadedFilePath());
        ImGui::Text("Duration: %s, Sample rate: %u Hz, Channels: %u (%s)",
                    Utils::formatTime(m_audioEngine.getDuration()).c_str(),
                    m_audioEngine.getSampleRate(),
                    m_audioEngine.getChannelCount(),
                    fileName.c_str());
    }
    else
    {
        ImGui::Text("No audio file loaded");
        ImGui::Text("Use File -> Open Audio File to load a track for practice.");
    }
}

void MainWindow::showGui()
{
    ImGui::Text("SongPractice - Audio Practice Tool");
    ImGui::Separator();

    if (m_audioEngine.hasAudio() && m_waveformDirty)
        updateWaveformData();

    renderWaveformArea();
    renderAudioInfo();
    renderAudioControls();
    renderMarkerControls();

    m_appState.playPosition = m_audioEngine.getCurrentTime();
}

void MainWindow::showMenus()
{
    // Add our custom File menu
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open Audio File...", "Ctrl+O"))
        {
            openAudioFile();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Load Track Settings..."))
        {
            loadTrackSettings();
        }

        if (ImGui::MenuItem("Save Track Settings..."))
        {
            saveTrackSettings();
        }

        ImGui::EndMenu();
    }
}

void MainWindow::openAudioFile()
{
    // Use portable-file-dialogs for file selection
    auto selection = pfd::open_file("Select Audio File",
                                   "",
                                   {"Audio Files", "*.wav *.mp3 *.flac *.ogg *.m4a *.aac",
                                    "All Files", "*"},
                                   pfd::opt::none).result();

    if (!selection.empty())
    {
        std::string filePath = selection[0];
        if (Utils::isAudioFile(filePath))
        {
            const std::string extension = Utils::getFileExtension(filePath);
            if (extension != "wav" && extension != "mp3")
            {
                HelloImGui::Log(HelloImGui::LogLevel::Warning,
                                "Currently supported formats: wav, mp3 (got: %s)",
                                extension.c_str());
                return;
            }

            if (m_audioEngine.loadAudioFile(filePath.c_str()))
            {
                m_appState.soundFilePath = filePath;
                m_waveformDirty = true;
                HelloImGui::Log(HelloImGui::LogLevel::Info, "Loaded audio file: %s",
                              Utils::getFileName(filePath).c_str());
            }
            else
            {
                HelloImGui::Log(HelloImGui::LogLevel::Error, "Failed to load audio file: %s",
                              Utils::getFileName(filePath).c_str());
            }
        }
        else
        {
            HelloImGui::Log(HelloImGui::LogLevel::Warning, "Unsupported file format: %s",
                          Utils::getFileName(filePath).c_str());
        }
    }
}

void MainWindow::renderAudioControls()
{
    ImGui::Spacing();
    ImGui::Text("Audio Controls:");

    const bool hasAudio = m_audioEngine.hasAudio();

    if (!hasAudio)
        ImGui::BeginDisabled();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(kTransportSpacing, 10.0f));

    if (showControlButton(ICON_FA_BACKWARD, "Rewind 1 second", true))
        m_audioEngine.seekBy(-kSeekStep);

    ImGui::SameLine();
    const bool isPlaying = m_audioEngine.isPlaying();
    const char* playIcon = isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY;
    if (showControlButton(playIcon, isPlaying ? "Pause" : "Play"))
    {
        if (isPlaying)
            m_audioEngine.pause();
        else
        {
            // rewind to start if at the end
            if (m_audioEngine.getCurrentTime() >= m_audioEngine.getDuration())
                m_audioEngine.seek(0.0f);
            m_audioEngine.play();
        }
    }

    ImGui::SameLine();

    if (showControlButton(ICON_FA_FORWARD, "Forward 1 second", true))
        m_audioEngine.seekBy(kSeekStep);

    ImGui::PopStyleVar();

    if (!hasAudio)
        ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0));
    ImGui::SameLine();

    const ImVec4 timeColor = isPlaying ? ImVec4(0.2f, 0.85f, 0.4f, 1.0f)
                                       : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, timeColor);
    ImGui::Text("%s / %s",
                Utils::formatTime(m_audioEngine.getCurrentTime()).c_str(),
                Utils::formatTime(m_audioEngine.getDuration()).c_str());
    ImGui::PopStyleColor();
}

void MainWindow::seekToPreviousMarker()
{
    if (m_appState.markers.empty())
    {
        m_audioEngine.seek(0.0f);
        return;
    }

    const float currentTime = m_audioEngine.getCurrentTime();
    const int idx = currentMarkerIndex();

    if (idx >= 0)
    {
        const float distanceToCurrentMarker = currentTime - m_appState.markers[idx].timeSeconds;

        if (distanceToCurrentMarker > 1.f)
        {
            // Head is far from current marker, seek to current marker
            m_audioEngine.seek(m_appState.markers[idx].timeSeconds);
        }
        else if (idx > 0)
        {
            // Head is close to current marker, seek to previous marker
            m_audioEngine.seek(m_appState.markers[idx - 1].timeSeconds);
        }
        else
        {
            // No previous marker, seek to start
            m_audioEngine.seek(0.0f);
        }
    }
    else
    {
        // No marker before current position, seek to start
        m_audioEngine.seek(0.0f);
    }
}

void MainWindow::seekToNextMarker()
{
    if (m_appState.markers.empty())
        return;

    const int idx = currentMarkerIndex();
    const int nextIdx = idx + 1;

    if (nextIdx < static_cast<int>(m_appState.markers.size()))
    {
        // Seek to next marker
        m_audioEngine.seek(m_appState.markers[nextIdx].timeSeconds);
    }
    else
    {
        // seek to end
        m_audioEngine.seek(m_audioEngine.getDuration());
    }
}

void MainWindow::renderMarkerControls()
{
    if (!m_audioEngine.hasAudio())
        return;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Markers:");

    const bool hasMarkers = !m_appState.markers.empty();
    if (!hasMarkers)
        ImGui::BeginDisabled();
    if (showControlButton(ICON_FA_BACKWARD_STEP, "seek to Previous Marker"))
        seekToPreviousMarker();
    ImGui::SameLine();
    if (showControlButton(ICON_FA_FORWARD_STEP, "seek to Next Marker"))
        seekToNextMarker();
    if (!hasMarkers)
        ImGui::EndDisabled();

    ImGui::SameLine();
    if (showControlButton(ICON_FA_PLUS, "Add Marker"))
    {
        Marker marker;
        marker.timeSeconds = m_audioEngine.getCurrentTime();
        marker.name = Utils::formatTime(marker.timeSeconds);
        m_appState.markers.push_back(marker);
        sortMarkers();
    }


    ImGui::BeginChild("Markers", HelloImGui::EmToVec2(0, 20));
    int markerToDelete = -1;
    for (int i = 0; i < m_appState.markers.size(); ++i)
    {
        ImGui::PushID(i);
        Marker& marker = m_appState.markers[i];
        ImGui::SetNextItemWidth(HelloImGui::EmSize(15.f));
        ImGui::InputText("Name", &marker.name);
        ImGui::SameLine(HelloImGui::EmSize(25.f));
        if (ImGui::Button("Delete Marker"))
            markerToDelete = i;
        ImGui::PopID();
    }
    ImGui::EndChild();
    if (markerToDelete >= 0)
        m_appState.markers.erase(m_appState.markers.begin() + markerToDelete);
}

void MainWindow::renderWaveformArea()
{
    ImGui::Spacing();
    ImGui::Text("Waveform Display:");
    ImGui::BeginChild("Waveform", ImVec2(0, 300), true);

    if (m_audioEngine.hasAudio() && m_waveformRenderer.hasWaveform())
    {
        ImPlot::SetNextAxesLimits(0.0, m_audioEngine.getDuration(), -1.0, 1.0, ImGuiCond_Once);
        float seekTime = 0.0f;
        std::vector<MarkerView> markerViews;
        markerViews.reserve(m_appState.markers.size());
        for (int i = 0; i < static_cast<int>(m_appState.markers.size()); ++i)
        {
            MarkerView mv;
            mv.label = m_appState.markers[i].name;
            mv.timeSeconds = m_appState.markers[i].timeSeconds;
            markerViews.push_back(std::move(mv));
        }

        if (m_waveformRenderer.draw("WaveformPlot",
                                     ImVec2(-1, -1),
                                     m_audioEngine.getCurrentTime(),
                                     seekTime,
                                     markerViews))
        {
            m_audioEngine.seek(seekTime);
        }
    }
    else
    {
        ImGui::TextDisabled("Load an audio file to see waveform");
    }

    ImGui::EndChild();
}

void MainWindow::updateWaveformData()
{
    if (m_audioEngine.hasAudio())
    {
        m_waveformRenderer.setWaveform(m_audioEngine.getAudioData(),
                                       m_audioEngine.getChannelCount(),
                                       m_audioEngine.getSampleRate());
    }
    else
    {
        m_waveformRenderer.clear();
    }
    m_waveformDirty = false;
}

void MainWindow::showStatus()
{
    // HelloImGui will handle the status bar layout, we just add content
    if (m_audioEngine.isPlaying())
    {
        ImGui::Text("Playing");
    }
    else
    {
        ImGui::Text("Ready");
    }
    
    // Add separator and file info
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();
    
    if (m_audioEngine.hasAudio())
    {
        ImGui::Text("%s - %s",
                   Utils::getFileName(m_audioEngine.loadedFilePath()).c_str(),
                   Utils::formatTime(m_audioEngine.getCurrentTime()).c_str());
    }
    else
    {
        ImGui::Text("No audio loaded");
    }
}

int MainWindow::currentMarkerIndex() const
{
    const float currentTime = m_audioEngine.getCurrentTime();
    int idx = -1;
    for (int i = 0; i < static_cast<int>(m_appState.markers.size()); ++i)
    {
        if (m_appState.markers[i].timeSeconds <= currentTime)
            idx = i;
        else
            break;
    }
    return idx;
}

void MainWindow::sortMarkers()
{
    std::sort(m_appState.markers.begin(), m_appState.markers.end(), [](const Marker& a, const Marker& b) {
        return a.timeSeconds < b.timeSeconds;
    });
}
