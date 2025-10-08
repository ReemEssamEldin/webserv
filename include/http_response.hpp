#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class HttpResponse
{
private:
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string getStatusMessage(int code) const;

public:
    HttpResponse();
    ~HttpResponse();

    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& content);

    std::string build() const;

    static HttpResponse createOkResponse(const std::string& content,
                                         const std::string& contentType);
    static HttpResponse createNotFoundResponse(
        const std::string& errorPage);
};

#endif
