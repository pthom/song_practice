#include "immapp/immapp.h"
#include "hello_imgui/hello_imgui.h"
#include "ui/MainWindow.h"

int main(int argc, char **argv)
{
    // Setup ImGui Bundle
    ImmApp::AddOnsParams addOnsParams;
    addOnsParams.withImplot = true;  // Enable ImPlot for waveform visualization
    
    // Create main window
    MainWindow mainWindow;
    
    // Application parameters
    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "SongPractice - Audio Practice Tool";
    runnerParams.appWindowParams.windowGeometry.size = {1200, 800};
    runnerParams.appWindowParams.restorePreviousGeometry = true;
    
    // Enable full screen dock space
    runnerParams.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenWindow;
    
    // Setup menu and status bar
    runnerParams.imGuiWindowParams.showMenuBar = true;
    runnerParams.imGuiWindowParams.showStatusBar = true;
    runnerParams.imGuiWindowParams.menuAppTitle = "SongPractice";
    
    // Setup callbacks using HelloImGui utilities
    runnerParams.callbacks.ShowGui = [&mainWindow]() {
        mainWindow.showGui();
    };
    runnerParams.imGuiWindowParams.showMenu_View = false;
    runnerParams.imGuiWindowParams.showMenu_App_Quit = false;
    runnerParams.callbacks.ShowMenus = [&mainWindow]() {
        mainWindow.showMenus();
    };
    runnerParams.callbacks.ShowStatus = [&mainWindow]() {
        mainWindow.showStatus();
    };

    // Run the application
    ImmApp::Run(runnerParams, addOnsParams);

    return 0;
}