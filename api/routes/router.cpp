#include "./../util/utils.h"
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include "./../services/services.h"
#include "./fileRouter.cpp"
#include "./httpErrors.cpp"

namespace routes {

    using RouteHandler = std::function<std::string()>;

    std::string cleanAndFormatRoute(std::string filePath) {
        // For removing .extensions from url
        if (filePath.empty() || filePath == "/" || filePath.back() == '/') {
            // Normalize filePath: remove trailing slash and append "index.html"
            if (filePath.back() == '/' && filePath.size() > 0) {
                filePath.pop_back();  
            }
            filePath += "/index.html";
        }
        return filePath;
    }


    class Router {
        private:
            std::unordered_map<std::string, RouteHandler> allRoutes;
            std::unordered_map<std::string, RouteHandler> getRoutes () {
                
                std::unordered_map<std::string, RouteHandler> routes = {
                    //define custom routes
                }; 
                std::vector<std::string> routeNames = services::getAllRouteNames();
                for (const std::string& it : routeNames) {
                    routes.insert({it, [it]() { return routes::fileRouter(it); }});
                }
                return routes;
            }



        public:
            Router(){
                this->allRoutes = this->getRoutes();
            }
            ~Router(){}
            std::string process (util::Request request) {
                std::string routeUrl = routes::cleanAndFormatRoute(request.getUrl());
                auto it = allRoutes.find(routeUrl);
                if (it != allRoutes.end()) {
                    return it->second();
                }

                return routes::httpErrors::badRequest();
            }
    };

}
