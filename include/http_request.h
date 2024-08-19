#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <unordered_map>

class HttpRequest {
public:
    HttpRequest(const std::string& request);
    std::string getHeader(const std::string& field) const;

private:
    std::unordered_map<std::string, std::string> headers_;
    void parseRequest(const std::string& request);
};

#endif // HTTP_REQUEST_H
