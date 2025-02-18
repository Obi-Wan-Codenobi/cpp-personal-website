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
                // pages.insert(path + std::to_string(entry.path().filename()));
                try {
                    file
                } catch (declaration) {
                
                }
                // strip directory
                std::string webPagePath =  fileName.substr(strlen(STATIC_FILES), fileName.length());
                std::cout << webPagePath << std::endl;
                pages.insert()
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

    std::string getFile(std::string &fileName)
    {
        try
        {
            return webPages.getPage("index");

        } catch (std::out_of_range e) 
        {
            std::cerr << "Error: " << e.what() << '\n';
        }

    }

}
