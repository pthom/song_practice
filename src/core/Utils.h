#pragma once
#include <string>

namespace Utils
{
    // Time formatting
    std::string formatTime(float seconds);
    
    // File path utilities
    std::string getFileName(const std::string& filePath);
    std::string getFileExtension(const std::string& filePath);
    bool isAudioFile(const std::string& filePath);
    bool stringEndsWith(const std::string& str, const std::string& suffix);
}