#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "route_config.hpp"
#include <string>
#include <vector>
#include <map>

class ServerConfig
{
private:
    std::string host;
    int port;
    std::string serverName;
    size_t clientMaxBodySize;
    std::map<int, std::string> errorPages;
    std::vector<RouteConfig> routes;
    std::string defaultRoot;

public:
    ServerConfig();
    ~ServerConfig();

    void setHost(const std::string& hostAddress);
    void setPort(int portNumber);
    void setServerName(const std::string& name);
    void setClientMaxBodySize(size_t size);
    void setErrorPage(int code, const std::string& path);
    void addRoute(const RouteConfig& route);
    void setDefaultRoot(const std::string& root);

    std::string getHost() const;
    int getPort() const;
    std::string getServerName() const;
    size_t getClientMaxBodySize() const;
    std::string getErrorPage(int code) const;
    const RouteConfig* findRoute(const std::string& path) const;
    std::string getDefaultRoot() const;
};

#endif
