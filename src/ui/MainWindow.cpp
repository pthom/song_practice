#include "MainWindow.h"
#include "imgui.h"
#include "implot/implot.h"
#include "hello_imgui/hello_imgui.h"
#include "core/Utils.h"
#include "portable_file_dialogs/portable_file_dialogs.h"
#include "hello_imgui/icons_font_awesome_6.h"

namespace
{
    constexpr float kSeekStep = 1.0f;
    constexpr ImVec2 kTransportButtonSize = ImVec2(40.0f, 40.0f);
    constexpr float kTransportSpacing = 14.0f;
}


MainWindow::MainWindow()
{
    m_audioEngine.initialize();
}

MainWindow::~MainWindow()
{
    m_audioEngine.shutdown();
}

void MainWindow::showGui()
{
    ImGui::Text("SongPractice - Audio Practice Tool");
    ImGui::Separator();

    if (m_audioEngine.hasAudio())
    {
        const std::string fileName = Utils::getFileName(m_audioEngine.loadedFilePath());
        ImGui::Text("Current file: %s", fileName.c_str());
        ImGui::Text("Duration: %s", Utils::formatTime(m_audioEngine.getDuration()).c_str());
        ImGui::Text("Sample rate: %u Hz", m_audioEngine.getSampleRate());
        ImGui::Text("Channels: %u", m_audioEngine.getChannelCount());
        if (m_waveformDirty)
        {
            updateWaveformData();
        }
    }
    else
    {
        ImGui::Text("No audio file loaded");
        ImGui::Text("Use File -> Open Audio File to load a track for practice.");
    }

    renderAudioControls();
    renderWaveformArea();

    // Show debug windows if requested
    if (m_showDemo)
        ImGui::ShowDemoWindow(&m_showDemo);
    if (m_showMetrics)
        ImGui::ShowMetricsWindow(&m_showMetrics);
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
        ImGui::EndMenu();
    }
    
    // Add custom View menu items
    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Demo Window", nullptr, &m_showDemo);
        ImGui::MenuItem("Metrics Window", nullptr, &m_showMetrics);
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

    auto transportButton = [](const char* icon, const char* tooltip, bool autorepeat = false) -> bool{
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
    };

    if (transportButton(ICON_FA_BACKWARD, "Rewind 1 second", true))
        m_audioEngine.seekBy(-kSeekStep);

    ImGui::SameLine();
    const bool isPlaying = m_audioEngine.isPlaying();
    const char* playIcon = isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY;
    if (transportButton(playIcon, isPlaying ? "Pause" : "Play"))
    {
        if (isPlaying)
            m_audioEngine.pause();
        else
            m_audioEngine.play();
    }

    ImGui::SameLine();
    if (transportButton(ICON_FA_STOP, "Stop"))
        m_audioEngine.stop();

    ImGui::SameLine();

    if (transportButton(ICON_FA_FORWARD, "Forward 1 second", true))
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

void MainWindow::renderWaveformArea()
{
    ImGui::Spacing();
    ImGui::Text("Waveform Display:");
    ImGui::BeginChild("Waveform", ImVec2(0, 300), true);

    if (m_audioEngine.hasAudio() && m_waveformRenderer.hasWaveform())
    {
        ImPlot::SetNextAxesLimits(0.0, m_audioEngine.getDuration(), -1.0, 1.0, ImGuiCond_Once);
        float seekTime = 0.0f;
        if (m_waveformRenderer.draw("WaveformPlot", ImVec2(-1, -1), m_audioEngine.getCurrentTime(), seekTime))
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
