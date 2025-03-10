#include "./../services/services.h"
#include <string>
#include "./../util/utils.h"
#include "./httpErrors.cpp"


namespace routes {
    std::string fileRouter (std::string filePath) {
        std::string headers;
        std::string response;

       
        if (util::isTextFile(filePath)) {
            std::string response_body = services::getTextFile(filePath);
            if (response_body.empty()) {
                return routes::httpErrors::notFound();
            }
            headers =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: " + util::getFileType(filePath) + "\r\n"
                "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
                "Connection: keep-alive\r\n"
                "\r\n";
            response = headers + response_body;
        } else {
            std::vector<std::byte> file_content = services::getBinaryFile(filePath);
            if (file_content.empty()) {
                return routes::httpErrors::notFound();
            }
            headers =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: " + util::getFileType(filePath) + "\r\n"
                "Content-Length: " + std::to_string(file_content.size()) + "\r\n"
                "Connection: keep-alive\r\n"
                "\r\n";
            response.reserve(headers.size() + file_content.size());
            response.append(headers);
            response.append(reinterpret_cast<const char*>(file_content.data()), file_content.size());
        }

        return response;

    }
}
