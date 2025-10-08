#include "http_request.hpp"
#include <sstream>
#include <algorithm>

HttpRequest::HttpRequest() : method(""), path(""), httpVersion(""), body("")
{
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::parseRequestLine(const std::string& line)
{
    std::istringstream iss(line);
    if (!(iss >> method >> path >> httpVersion))
        return false;
    return true;
}

bool HttpRequest::parseHeader(const std::string& line)
{
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos)
        return false;

    std::string key = line.substr(0, colonPos);
    std::string value = line.substr(colonPos + 1);

    // Trim leading whitespace from value
    size_t start = value.find_first_not_of(" \t");
    if (start != std::string::npos)
        value = value.substr(start);

    headers[key] = value;
    return true;
}

bool HttpRequest::parse(const std::string& rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;
    bool firstLine = true;
    bool headersSection = true;

    while (std::getline(stream, line))
    {
        // Remove carriage return if present
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);

        if (firstLine)
        {
            if (!parseRequestLine(line))
                return false;
            firstLine = false;
        }
        else if (line.empty())
        {
            // Empty line marks end of headers
            headersSection = false;
            // Rest is body (if any)
            std::string remaining;
            std::getline(stream, remaining, '\0');
            body = remaining;
            break;
        }
        else if (headersSection)
        {
            if (!parseHeader(line))
                return false;
        }
    }

    return true;
}

std::string HttpRequest::getMethod() const
{
    return method;
}

std::string HttpRequest::getPath() const
{
    return path;
}

std::string HttpRequest::getHttpVersion() const
{
    return httpVersion;
}

std::string HttpRequest::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = headers.find(key);
    if (it != headers.end())
        return it->second;
    return "";
}

std::string HttpRequest::getBody() const
{
    return body;
}
