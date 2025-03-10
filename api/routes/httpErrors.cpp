#ifndef HTTP_ERRORS
#define HTTP_ERRORS

#include <string>

// LLM generated

namespace routes {
    namespace httpErrors {
        // Helper function to build an HTTP error response
        static std::string buildErrorResponse(const std::string& statusLine, const std::string& message) {
            std::string contentLength = std::to_string(message.size());
            std::string response =
                statusLine + "\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: " + contentLength + "\r\n"
                "Connection: keep-alive\r\n"
                "\r\n" +
                message;
            return response;
        }

        // 400 Bad Request
        static std::string badRequest(const std::string& customMessage = "Bad Request") {
            return buildErrorResponse("HTTP/1.1 400 Bad Request", customMessage);
        }

        // 403 Forbidden
        static std::string forbidden(const std::string& customMessage = "Forbidden") {
            return buildErrorResponse("HTTP/1.1 403 Forbidden", customMessage);
        }

        // 404 Not Found
        static std::string notFound(const std::string& customMessage = "File not found") {
            return buildErrorResponse("HTTP/1.1 404 Not Found", customMessage);
        }

        // 405 Method Not Allowed
        static std::string methodNotAllowed(const std::string& customMessage = "Method Not Allowed") {
            return buildErrorResponse("HTTP/1.1 405 Method Not Allowed", customMessage);
        }

        // 500 Internal Server Error
        static std::string internalServerError(const std::string& customMessage = "Internal Server Error") {
            return buildErrorResponse("HTTP/1.1 500 Internal Server Error", customMessage);
        }

        // 501 Not Implemented
        static std::string notImplemented(const std::string& customMessage = "Not Implemented") {
            return buildErrorResponse("HTTP/1.1 501 Not Implemented", customMessage);
        }
    }  // namespace HttpErrors
}  // namespace routes

#endif
