#include "http_response.h"

HttpResponse::HttpResponse() : status_("200 OK") {}

void HttpResponse::setStatus(const std::string& status) {
    status_ = status;
}

void HttpResponse::setHeader(const std::string& field, const std::string& value) {
    headers_[field] = value;
}

std::string HttpResponse::getResponse() const {
    std::string response = "HTTP/1.1 " + status_ + "\r\n";
    for (const auto& header : headers_) {
        response += header.first + ": " + header.second + "\r\n";
    }
    response += "\r\n";
    return response;
}
