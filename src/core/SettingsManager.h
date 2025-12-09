#pragma once

#include <string>
#include <vector>

struct Marker;
struct ApplicationState;

class SettingsManager
{
public:
    SettingsManager();
    ~SettingsManager();

    // Global settings file management
    bool loadGlobalSettings(ApplicationState& state);
    bool saveGlobalSettings(const ApplicationState& state);

    // Per-track settings (for future implementation)
    bool loadTrackSettings(const std::string& trackPath, ApplicationState& state);
    bool saveTrackSettings(const std::string& trackPath, const ApplicationState& state);

    // Get paths
    std::string getGlobalSettingsPath() const;
    std::string getTrackSettingsPath(const std::string& trackPath) const;

private:
    std::string getExecutableDirectory() const;
    void logError(const std::string& message) const;

    static constexpr const char* GLOBAL_SETTINGS_FILENAME = "songpractice-settings.json";
    static constexpr const char* TRACK_SETTINGS_EXTENSION = ".songpractice.json";
};
