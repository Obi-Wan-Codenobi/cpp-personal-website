#ifndef FORMAT_RESPONSE
#define FORMAT_RESPONSE

#include <string>
#include <unordered_map>
#include "fileReading.cpp"

namespace util {
    static const std::unordered_map<std::string, std::string> fileTypes = {
        {".html", "text/html"},
        {".js", "application/javascript"},
        {".css", "text/css"},
        {".txt", "text/plain"},
        {".jpg", "img/jpeg"},
        {".jpeg", "img/jpeg"},
        {".png", "img/png"},
        {".svg", "image/svg+xml"}
    };

    // Helper function to get file type
    std::string getFileType(const std::string& filePath) {
        std::string extension = util::getFileExtension(filePath);
        auto it = fileTypes.find(extension);
        if (it != fileTypes.end()) {
            return it->second;
        }
        return "application/octet-stream";  // Fallback
    }
}

#endif
