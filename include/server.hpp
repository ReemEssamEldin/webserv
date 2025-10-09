#ifndef SERVER_HPP
#define SERVER_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include "file_reader.hpp"
#include "server_config.hpp"
#include "directory_listing.hpp"
#include <string>

class Server
{
private:
    int serverFd;
    ServerConfig config;
    FileReader fileReader;
    DirectoryListing dirListing;

    bool initSocket();
    bool bindAndListen();
    void handleClient(int clientFd);
    HttpResponse processRequest(const HttpRequest& request);
    HttpResponse handleGetRequest(const HttpRequest& request,
                                   const RouteConfig* route);
    HttpResponse handlePostRequest(const HttpRequest& request,
                                    const RouteConfig* route);
    HttpResponse handleDeleteRequest(const HttpRequest& request,
                                      const RouteConfig* route);
    std::string resolveFilePath(const std::string& requestPath,
                                 const RouteConfig* route) const;
    bool isDirectory(const std::string& path) const;

public:
    Server();
    Server(const ServerConfig& configuration);
    ~Server();

    void setConfig(const ServerConfig& configuration);
    bool start();
    void run();
    void stop();
};

#endif
