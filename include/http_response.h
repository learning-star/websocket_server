#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>

class HttpResponse {
public:
    HttpResponse();
    void setStatus(const std::string& status);
    void setHeader(const std::string& field, const std::string& value);
    std::string getResponse() const;

private:
    std::string status_;
    std::unordered_map<std::string, std::string> headers_;
};

#endif // HTTP_RESPONSE_H
