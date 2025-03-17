#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <vector>
#include "./../config/environmentVars.h"
#include "./../util/utils.h"

namespace services
{
    class WebPages {
        private:
            std::unordered_map<std::string, std::string> pages;
            std::unordered_map<std::string, std::vector<std::byte>> binary_files;
            void loadWebPages(std::string path) 
            {
                try 
                {
                    for (const auto& entry : std::filesystem::directory_iterator(path)) 
                    {
                        std::string file = path + "/" + entry.path().filename().string();
                        entry.is_directory() ?
                                loadWebPages(file) :
                                insertWebPage(file);

                    }
                } catch (const std::filesystem::filesystem_error& e) 
                {
                    std::cerr << "Error: " << e.what() << '\n';
                }
            }

            void insertWebPage(const std::string fileName) 
            {
                // strip directory
                std::string webPagePath =  fileName.substr(strlen(STATIC_FILES), fileName.length());
                
                if (!util::isTextFile(webPagePath)) {
                    std::cout << "BINARY: " << webPagePath << std::endl;
                    std::vector<std::byte> file_content = util::getBinaryFile(fileName);
                    if(file_content.size() <=0){
                        std::cerr<<"Error: " <<"Binary file did not load"<<std::endl;
                    }
                    std::vector<std::byte> compressed_data =  util::compressGzip(file_content);
                    binary_files.insert({webPagePath, compressed_data});
                    
                }
                else {
                    std::cout << "TEXT: "<< webPagePath << std::endl;

                    std::string file_content = util::getTextFile(fileName);
                    if(file_content.size() <=0){
                        std::cerr<<"Error: " <<"Text file did not load"<<std::endl;
                    }
                    std::string compressed_data =  util::compressGzip(file_content);
                    pages.insert({webPagePath, compressed_data});
                }
            }


        public:
            WebPages()
            {
                loadWebPages(STATIC_FILES);
            }

            ~WebPages(){}

            std::string& getPage(const std::string& pageName) 
            {
                // Throws std::out_of_range if not found
                return pages.at(pageName);
            }

            std::vector<std::byte>& getFile(const std::string& pageName){
                // Throws std::out_of_range if not found
                return binary_files.at(pageName);
            }

            std::vector<std::string> getAllFileNames () {
                std::vector<std::string> fileNames;
                for(const auto& pair : this->pages) {
                    fileNames.push_back(pair.first);
                }
                for(const auto& pair : this->binary_files){
                    fileNames.push_back(pair.first);
                }
                return fileNames;
            }
    };

    WebPages webPages = WebPages();

    std::vector<std::string> getAllRouteNames() {
        return webPages.getAllFileNames();
    }

    std::vector<std::byte> getBinaryFile(std::string fileName){
        try
        {
            return webPages.getFile(fileName);

        } catch (std::out_of_range e) 
        {
            std::cerr << "Error: " << e.what() << '\n'<<
                "BINARY FILE REQUESTED: " << fileName << std::endl;
        }
        return std::vector<std::byte>(0);

    }
    std::string getTextFile(const std::string &fileName)
    {
        try
        {
            return webPages.getPage(fileName);

        } catch (std::out_of_range e) 
        {
            std::cerr << "Error: " << e.what() << '\n'<<
                "TEXT FILE REQUESTED: " << fileName << std::endl;
        }

        return R"RAW(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Error - Something Went Wrong</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #121212;
            color: #fff;
            text-align: center;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .error-container {
            padding: 20px;
            background: #1e1e1e;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(255, 0, 0, 0.5);
            animation: glitch 1s infinite alternate;
        }
        h1 {
            color: #ff4c4c;
        }
        p {
            margin: 10px 0;
        }
        button {
            padding: 10px 20px;
            background-color: #ff4c4c;
            border: none;
            color: white;
            cursor: pointer;
            font-size: 16px;
            border-radius: 5px;
            transition: 0.3s;
        }
        button:hover {
            background-color: #ff1c1c;
        }
        @keyframes glitch {
            0% { transform: translateX(2px); }
            100% { transform: translateX(-2px); }
        }
    </style>
    <script>
        function reloadPage() {
            location.reload();
        }
    </script>
</head>
<body>
    <div class="error-container">
        <h1>Oops! Something Went Wrong</h1>
        <p>We're sorry, but an error occurred.</p>
        <button onclick="reloadPage()">Try Again</button>
    </div>
</body>
</html>
)RAW";

    }

}
