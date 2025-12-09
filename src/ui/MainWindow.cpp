#include "MainWindow.h"
#include "imgui.h"
#include "hello_imgui/hello_imgui.h"
#include "core/Utils.h"
#include "portable_file_dialogs/portable_file_dialogs.h"

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
    // This will be called by HelloImGui as the main GUI function
    // Main content area - HelloImGui handles the docking automatically
    if (ImGui::Begin("Audio Practice"))
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
        }
        else
        {
            ImGui::Text("No audio file loaded");
            ImGui::Text("Use File -> Open Audio File to load a track for practice.");
        }
        
        renderAudioControls();
        renderWaveformArea();
    }
    ImGui::End();
    
    // Show debug windows if requested
    if (m_showDemo)
        ImGui::ShowDemoWindow(&m_showDemo);
    if (m_showMetrics)
        ImGui::ShowMetricsWindow(&m_showMetrics);
}

void MainWindow::showMenus()
{
    auto& runnerParams = *HelloImGui::GetRunnerParams();
    HelloImGui::ShowAppMenu(runnerParams);
    HelloImGui::ShowViewMenu(runnerParams);
    
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
    
    if (!hasAudio) ImGui::BeginDisabled();
    
    if (ImGui::Button("Play"))
    {
        m_audioEngine.play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        m_audioEngine.pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        m_audioEngine.stop();
    }
    
    if (!hasAudio) ImGui::EndDisabled();
    
    // Time display
    if (hasAudio)
    {
        ImGui::SameLine();
        ImGui::Text("Time: %s / %s", 
                   Utils::formatTime(m_audioEngine.getCurrentTime()).c_str(),
                   Utils::formatTime(m_audioEngine.getDuration()).c_str());
    }
}

void MainWindow::renderWaveformArea()
{
    ImGui::Spacing();
    ImGui::Text("Waveform Display:");
    ImGui::BeginChild("Waveform", ImVec2(0, 200), true);
    
    if (m_audioEngine.hasAudio())
    {
        const std::string filePath = m_audioEngine.loadedFilePath();
        const std::string fileName = Utils::getFileName(filePath);
        ImGui::Text("Waveform visualization will appear here");
        ImGui::Text("File: %s", fileName.c_str());
        ImGui::Text("Format: %s", Utils::getFileExtension(filePath).c_str());
        ImGui::Text("Samples: %zu (frames: %llu)",
                    m_audioEngine.getAudioData().size(),
                    static_cast<unsigned long long>(m_audioEngine.getFrameCount()));
    }
    else
    {
        ImGui::TextDisabled("Load an audio file to see waveform");
    }
    
    ImGui::EndChild();
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