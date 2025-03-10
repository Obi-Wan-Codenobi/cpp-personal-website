#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>

namespace util {

    class Request {
        private:
            std::string request_type;
            std::string url;
            std::string http_version;
            std::unordered_map<std::string, std::string> headers;
            std::string body;

            void parse_request(const std::string &raw_request){
                std::istringstream stream(raw_request);
                std::string buffer;

                if (std::getline(stream, buffer)) {
                    std::istringstream request_buffer(buffer);
                    request_buffer >> this->request_type >> this->url >> this->http_version;
                }
                

                while (std::getline(stream, buffer)) {
                    size_t colon_index = buffer.find(": ");
                    if (colon_index == std::string::npos) continue;
                    std::string name = buffer.substr(0,colon_index);
                    std::string value = buffer.substr(colon_index + 2);

                    this->headers.insert({name, value});
                }

                auto it = this->headers.find("Content-Length");
                if (it != this->headers.end()) {
                    size_t body_length = std::stoul(it->second);
                    this->body.resize(body_length);
                    stream.read(&this->body[0], body_length);

                }

            }


        public:
            Request(std::string raw_request) {
                parse_request(raw_request);
            }

            ~Request(){}

            std::string getMethod() const { return this->request_type; }
            std::string getUrl() const { return this->url; }
            std::string getHttpVersion() const { return this->http_version; }
            std::string getHeader(const std::string &key) const {
                auto it = this->headers.find(key);
                return (it != this->headers.end()) ? it->second : "";
            }
            std::string getBody() const { return this->body; }


            void printRequest() const {
                std::cout << "Method: " << this->request_type << "\n";
                std::cout << "URL: " << this->url << "\n";
                std::cout << "HTTP Version: " << this->http_version << "\n";
                std::cout << "Headers:\n";
                for (const auto &header : this->headers) {
                    std::cout << "  " << header.first << ": " << header.second << "\n";
                }
                std::cout << "Body:\n" << this->body << "\n";
            }

            std::string getRequestString() const {
                std::ostringstream oss;
                oss << "Method: " << this->request_type << "\n";
                oss << "URL: " << this->url << "\n";
                oss << "HTTP Version: " << this->http_version << "\n";
                oss << "Headers:\n";
                for (const auto& header : this->headers) {
                    oss << "  " << header.first << ": " << header.second << "\n";
                }
                oss << "Body:\n" << this->body << "\n";
                return oss.str();
            }

            std::string getRequestWithoutHeadersString() const {
                std::ostringstream oss;
                oss << this->request_type << " ";
                oss << this->url << " ";
                oss << this->http_version << "\n";
                return oss.str();
            }

    };
}
