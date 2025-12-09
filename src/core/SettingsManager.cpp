#include "SettingsManager.h"
#include "ui/MainWindow.h"  // For ApplicationState and Marker structs
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
    #include <unistd.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

using json = nlohmann::json;

SettingsManager::SettingsManager() = default;
SettingsManager::~SettingsManager() = default;

bool SettingsManager::loadGlobalSettings(ApplicationState& state)
{
    const std::string settingsPath = getGlobalSettingsPath();

    if (!std::filesystem::exists(settingsPath))
    {
        // No settings file exists, use defaults
        state = ApplicationState{};
        return true;
    }

    return loadSettingsFromFile(settingsPath, state);
}

bool SettingsManager::saveGlobalSettings(const ApplicationState& state)
{
    return saveSettingsToFile(getGlobalSettingsPath(), state);
}

bool SettingsManager::loadTrackSettings(const std::string& settingsFilePath, ApplicationState& state)
{
    return loadSettingsFromFile(settingsFilePath, state);
}

bool SettingsManager::saveTrackSettings(const std::string& settingsFilePath, const ApplicationState& state)
{
    return saveSettingsToFile(settingsFilePath, state);
}

std::string SettingsManager::getGlobalSettingsPath() const
{
    return getExecutableDirectory() + "/" + GLOBAL_SETTINGS_FILENAME;
}

std::string SettingsManager::getTrackSettingsPath(const std::string& trackPath) const
{
    std::filesystem::path path(trackPath);
    std::string stem = path.stem().string();
    return path.parent_path().string() + "/" + stem + TRACK_SETTINGS_EXTENSION;
}

std::string SettingsManager::getExecutableDirectory() const
{
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);
    return fullPath.substr(0, fullPath.find_last_of("\\"));
#elif defined(__APPLE__)
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
    {
        std::string fullPath(path);
        return fullPath.substr(0, fullPath.find_last_of("/"));
    }
    return ".";
#else
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1)
    {
        path[len] = '\0';
        std::string fullPath(path);
        return fullPath.substr(0, fullPath.find_last_of("/"));
    }
    return ".";
#endif
}

void SettingsManager::logError(const std::string& message) const
{
    std::cerr << "SettingsManager Error: " << message << std::endl;
}

bool SettingsManager::loadSettingsFromFile(const std::string& filePath, ApplicationState& state) const
{
    try
    {
        if (!std::filesystem::exists(filePath))
        {
            logError("Settings file does not exist: " + filePath);
            return false;
        }

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            logError("Failed to open settings file: " + filePath);
            return false;
        }

        json j;
        file >> j;

        // Load soundFilePath
        if (j.contains("soundFilePath") && j["soundFilePath"].is_string())
        {
            state.soundFilePath = j["soundFilePath"].get<std::string>();
        }

        // Load playPosition
        if (j.contains("playPosition") && j["playPosition"].is_number())
        {
            state.playPosition = j["playPosition"].get<float>();
        }

        // Load markers
        if (j.contains("markers") && j["markers"].is_array())
        {
            state.markers.clear();
            for (const auto& markerJson : j["markers"])
            {
                Marker marker;
                if (markerJson.contains("name") && markerJson["name"].is_string())
                {
                    marker.name = markerJson["name"].get<std::string>();
                }
                if (markerJson.contains("timeSeconds") && markerJson["timeSeconds"].is_number())
                {
                    marker.timeSeconds = markerJson["timeSeconds"].get<float>();
                }
                state.markers.push_back(marker);
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        logError("Exception loading settings from " + filePath + ": " + std::string(e.what()));
        return false;
    }
}

bool SettingsManager::saveSettingsToFile(const std::string& filePath, const ApplicationState& state) const
{
    try
    {
        json j;

        // Save soundFilePath
        j["soundFilePath"] = state.soundFilePath;

        // Save playPosition
        j["playPosition"] = state.playPosition;

        // Save markers
        json markersJson = json::array();
        for (const auto& marker : state.markers)
        {
            json markerJson;
            markerJson["name"] = marker.name;
            markerJson["timeSeconds"] = marker.timeSeconds;
            markersJson.push_back(markerJson);
        }
        j["markers"] = markersJson;

        std::ofstream file(filePath);
        if (!file.is_open())
        {
            logError("Failed to create settings file: " + filePath);
            return false;
        }

        file << j.dump(4); // Pretty print with 4-space indentation
        return true;
    }
    catch (const std::exception& e)
    {
        logError("Exception saving settings to " + filePath + ": " + std::string(e.what()));
        return false;
    }
}
