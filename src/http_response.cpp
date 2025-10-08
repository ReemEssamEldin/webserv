#include "http_response.hpp"
#include <sstream>

HttpResponse::HttpResponse() : statusCode(200), statusMessage("OK"), body("")
{
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::getStatusMessage(int code) const
{
    switch (code)
    {
        case 200: return "OK";
        case 404: return "Not Found";
        case 400: return "Bad Request";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

void HttpResponse::setStatus(int code)
{
    statusCode = code;
    statusMessage = getStatusMessage(code);
}

void HttpResponse::setHeader(const std::string& key,
                              const std::string& value)
{
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& content)
{
    body = content;
}

std::string HttpResponse::build() const
{
    std::ostringstream response;

    // Status line
    response << "HTTP/1.0 " << statusCode << " "
             << statusMessage << "\r\n";

    // Headers
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }

    // Empty line before body
    response << "\r\n";

    // Body
    response << body;

    return response.str();
}

HttpResponse HttpResponse::createOkResponse(const std::string& content,
                                            const std::string& contentType)
{
    HttpResponse response;
    response.setStatus(200);
    response.setHeader("Content-Type", contentType);

    std::ostringstream contentLength;
    contentLength << content.length();
    response.setHeader("Content-Length", contentLength.str());

    response.setBody(content);
    return response;
}

HttpResponse HttpResponse::createNotFoundResponse(
    const std::string& errorPage)
{
    HttpResponse response;
    response.setStatus(404);
    response.setHeader("Content-Type", "text/html");

    std::string body = errorPage.empty()
        ? "<html><body><h1>404 Not Found</h1></body></html>"
        : errorPage;

    std::ostringstream contentLength;
    contentLength << body.length();
    response.setHeader("Content-Length", contentLength.str());

    response.setBody(body);
    return response;
}
