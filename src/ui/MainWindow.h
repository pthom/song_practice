#pragma once

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();
    
    void render();
    
private:
    void renderMenuBar();
    void renderMainArea();
    void renderStatusBar();
    
    // UI state
    bool m_showDemo = false;
    bool m_showMetrics = false;
};