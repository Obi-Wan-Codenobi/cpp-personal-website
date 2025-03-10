#ifndef FILE_READING
#define FILE_READING

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstddef>

namespace util {

    //taken from llm

    std::string getFileExtension(const std::string& filePath) {
        size_t dotIndex = filePath.find_last_of(".");
        if (dotIndex == std::string::npos) return "";
        return filePath.substr(dotIndex);
    }

    bool isTextFile(const std::string& filePath) {
        std::string ext = util::getFileExtension(filePath);
        return ext == ".html" || ext == ".js" || ext == ".css" || ext == ".txt";
    }

    std::string getTextFile(const std::string& filePath) {
        std::ifstream file(filePath); 
        if (!file) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }


    std::vector<std::byte> getBinaryFile(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            return std::vector<std::byte>();  
        }

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<std::byte> buffer(size);

        if (size > 0) {
            std::vector<char> temp(size);
            file.read(temp.data(), size);
            for (std::streamsize i = 0; i < size; ++i) {
                buffer[i] = static_cast<std::byte>(temp[i]);
            }
        }

        file.close();
        return buffer;
    }
}

#endif
