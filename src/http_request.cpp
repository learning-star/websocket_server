#include "http_request.h"
#include <sstream>

HttpRequest::HttpRequest(const std::string& request) {
    parseRequest(request);
}

void HttpRequest::parseRequest(const std::string& request) {
    std::istringstream stream(request);
    std::string line;
    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            headers_[key] = value;
        }
    }
}

std::string HttpRequest::getHeader(const std::string& field) const {
    auto it = headers_.find(field);
    return it != headers_.end() ? it->second : "";
}
