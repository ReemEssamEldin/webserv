#include "server_config.hpp"
#include <algorithm>

ServerConfig::ServerConfig() : host("0.0.0.0"), port(8080),
                               serverName("webserv"),
                               clientMaxBodySize(1048576),
                               defaultRoot("www")
{
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::setHost(const std::string& hostAddress)
{
    host = hostAddress;
}

void ServerConfig::setPort(int portNumber)
{
    port = portNumber;
}

void ServerConfig::setServerName(const std::string& name)
{
    serverName = name;
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
    clientMaxBodySize = size;
}

void ServerConfig::setErrorPage(int code, const std::string& path)
{
    errorPages[code] = path;
}

void ServerConfig::addRoute(const RouteConfig& route)
{
    routes.push_back(route);
}

void ServerConfig::setDefaultRoot(const std::string& root)
{
    defaultRoot = root;
}

std::string ServerConfig::getHost() const
{
    return host;
}

int ServerConfig::getPort() const
{
    return port;
}

std::string ServerConfig::getServerName() const
{
    return serverName;
}

size_t ServerConfig::getClientMaxBodySize() const
{
    return clientMaxBodySize;
}

std::string ServerConfig::getErrorPage(int code) const
{
    std::map<int, std::string>::const_iterator it = errorPages.find(code);
    if (it != errorPages.end())
        return it->second;
    return "";
}

const RouteConfig* ServerConfig::findRoute(const std::string& path) const
{
    const RouteConfig* bestMatch = NULL;
    size_t bestMatchLen = 0;

    std::vector<RouteConfig>::const_iterator it;
    for (it = routes.begin(); it != routes.end(); ++it)
    {
        std::string routePath = it->getPath();
        size_t routeLen = routePath.length();

        if (path.compare(0, routeLen, routePath) == 0)
        {
            if (routeLen > bestMatchLen)
            {
                bestMatch = &(*it);
                bestMatchLen = routeLen;
            }
        }
    }

    return bestMatch;
}

std::string ServerConfig::getDefaultRoot() const
{
    return defaultRoot;
}
