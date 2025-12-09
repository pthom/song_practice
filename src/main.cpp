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
    
    // Setup GUI callback
    runnerParams.callbacks.ShowGui = [&mainWindow]() {
        mainWindow.render();
    };

    // Run the application
    ImmApp::Run(runnerParams, addOnsParams);

    return 0;
}