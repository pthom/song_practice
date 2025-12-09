#include "Utils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Utils
{
    std::string formatTime(float seconds)
    {
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << minutes 
            << ":" << std::setw(2) << secs;
        return oss.str();
    }
    
    std::string getFileName(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of("/\\");
        if (pos == std::string::npos)
            return filePath;
        return filePath.substr(pos + 1);
    }
    
    std::string getFileExtension(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of('.');
        if (pos == std::string::npos)
            return "";
        
        std::string ext = filePath.substr(pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    
    bool isAudioFile(const std::string& filePath)
    {
        std::string ext = getFileExtension(filePath);
        return (ext == "wav" || ext == "mp3" || ext == "flac" || 
                ext == "ogg" || ext == "m4a" || ext == "aac");
    }

    bool stringEndsWith(const std::string& str, const std::string& suffix)
    {
        if (str.length() < suffix.length())
            return false;
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
}