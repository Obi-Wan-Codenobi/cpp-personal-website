#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "./../config/environmentVars.h"

namespace services
{
    class WebPages {
        private:
            std::unordered_map<std::string, std::string> pages;
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
                std::string file_content;
                try {
                    std::fstream f(fileName);
                    std::string buffer;
                    
                    while (std::getline(f, buffer)) {
                        file_content += buffer;
                    }
                    f.close();
                } catch (...) {
                    std::cout<<"Error occurred reading in files" <<std::endl;
                }
                // strip directory
                std::string webPagePath =  fileName.substr(strlen(STATIC_FILES), fileName.length());
                std::cout << webPagePath << std::endl;
                //std::pair<std::string,std::string> new_pair (webPagePath, file_content);
                pages.insert({webPagePath, file_content});
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

    };

    WebPages webPages = WebPages();

    std::string getFile(const std::string &fileName)
    {
        try
        {
            return webPages.getPage(fileName);

        } catch (std::out_of_range e) 
        {
            std::cerr << "Error: " << e.what() << '\n'<<
                "FILE REQUESTED: " << fileName << std::endl;
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
