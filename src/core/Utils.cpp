#include "Utils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <unistd.h>
#else
    #include <unistd.h>
#endif

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

    std::string getExecutableDirectory()
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
}