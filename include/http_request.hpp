#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

class HttpRequest
{
private:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    bool parseRequestLine(const std::string& line);
    bool parseHeader(const std::string& line);

public:
    HttpRequest();
    ~HttpRequest();

    bool parse(const std::string& rawRequest);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getHttpVersion() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;
};

#endif
