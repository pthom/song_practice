#include "MainWindow.h"
#include "imgui.h"
#include "hello_imgui/hello_imgui.h"

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::render()
{
    renderMenuBar();
    renderMainArea();
    renderStatusBar();
    
    // Show debug windows if requested
    if (m_showDemo)
        ImGui::ShowDemoWindow(&m_showDemo);
    if (m_showMetrics)
        ImGui::ShowMetricsWindow(&m_showMetrics);
}

void MainWindow::renderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Audio File...", "Ctrl+O"))
            {
                // TODO: Implement file opening
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                // TODO: Implement save on exit
                HelloImGui::GetRunnerParams()->appShallExit = true;
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Demo Window", nullptr, &m_showDemo);
            ImGui::MenuItem("Metrics", nullptr, &m_showMetrics);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void MainWindow::renderMainArea()
{
    // Get the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    // Set up the main docking space
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("MainDockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    // Create docking space
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    // Main content area
    if (ImGui::Begin("Audio Practice"))
    {
        ImGui::Text("SongPractice - Audio Practice Tool");
        ImGui::Separator();
        
        ImGui::Text("Welcome to SongPractice!");
        ImGui::Text("Use File -> Open Audio File to load a track for practice.");
        
        // Placeholder for future audio controls
        ImGui::Spacing();
        ImGui::Text("Audio Controls:");
        if (ImGui::Button("Play"))
        {
            // TODO: Implement play
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            // TODO: Implement pause
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            // TODO: Implement stop
        }
        
        // Placeholder for waveform
        ImGui::Spacing();
        ImGui::Text("Waveform Display:");
        ImGui::BeginChild("Waveform", ImVec2(0, 200), true);
        ImGui::Text("Waveform visualization will appear here");
        ImGui::EndChild();
    }
    ImGui::End();
    
    ImGui::End(); // MainDockSpace
}

void MainWindow::renderStatusBar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    // Position status bar at the bottom
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - 25));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 25));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    
    if (ImGui::Begin("StatusBar", nullptr, flags))
    {
        ImGui::Text("Ready");
        ImGui::SameLine(viewport->WorkSize.x - 150);
        ImGui::Text("No audio loaded");
    }
    ImGui::End();
}