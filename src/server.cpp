#include "server.hpp"
#include "socket_utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <fstream>

Server::Server() : serverFd(-1)
{
    config.setPort(8080);
    config.setDefaultRoot("www");
}

Server::Server(const ServerConfig& configuration)
    : serverFd(-1), config(configuration)
{
}

Server::~Server()
{
    stop();
}

void Server::setConfig(const ServerConfig& configuration)
{
    config = configuration;
}

bool Server::initSocket()
{
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        std::cerr << "Error: socket creation failed: "
                  << strerror(errno) << std::endl;
        return false;
    }

    if (setNonBlocking(serverFd) < 0)
    {
        std::cerr << "Error: failed to set non-blocking: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        serverFd = -1;
        return false;
    }

    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR,
                   (const void*)&opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error: setsockopt failed: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        serverFd = -1;
        return false;
    }

    return true;
}

bool Server::bindAndListen()
{
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, config.getHost().c_str(),
                  &serverAddr.sin_addr) <= 0)
    {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    }

    serverAddr.sin_port = htons(config.getPort());

    if (bind(serverFd, (struct sockaddr*)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error: bind failed: "
                  << strerror(errno) << std::endl;
        return false;
    }

    if (listen(serverFd, 10) < 0)
    {
        std::cerr << "Error: listen failed: "
                  << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Server::isDirectory(const std::string& path) const
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) < 0)
        return false;
    return S_ISDIR(fileStat.st_mode);
}

std::string Server::resolveFilePath(const std::string& requestPath,
                                     const RouteConfig* route) const
{
    std::string root;

    if (route && !route->getRoot().empty())
        root = route->getRoot();
    else
        root = config.getDefaultRoot();

    std::string path = requestPath;

    if (route && !route->getPath().empty() && route->getPath() != "/")
    {
        size_t routeLen = route->getPath().length();
        if (path.compare(0, routeLen, route->getPath()) == 0)
            path = path.substr(routeLen);
    }

    if (path.empty() || path == "/")
        path = "/" + (route ? route->getIndexFile() : "index.html");

    return root + path;
}

HttpResponse Server::handleGetRequest(const HttpRequest& request,
                                       const RouteConfig* route)
{
    std::string path = request.getPath();

    if (route && !route->getRedirect().empty())
    {
        HttpResponse response;
        response.setStatus(301);
        response.setHeader("Location", route->getRedirect());
        response.setBody("");
        return response;
    }

    std::string filePath = resolveFilePath(path, route);

    if (isDirectory(filePath))
    {
        std::string indexPath = filePath;
        if (indexPath[indexPath.length() - 1] != '/')
            indexPath += "/";
        indexPath += (route ? route->getIndexFile() : "index.html");

        if (fileReader.fileExists(indexPath.substr(
                config.getDefaultRoot().length())))
        {
            filePath = indexPath;
        }
        else if (route && route->getAutoindex())
        {
            std::string listing = dirListing.generateListing(
                filePath, path);
            if (!listing.empty())
                return HttpResponse::createOkResponse(
                    listing, "text/html");
        }
    }

    fileReader.setRootDirectory(config.getDefaultRoot());

    std::string relativePath = filePath.substr(
        config.getDefaultRoot().length());

    if (!fileReader.fileExists(relativePath))
    {
        std::string errorPage = config.getErrorPage(404);
        if (!errorPage.empty())
            errorPage = fileReader.readFile(errorPage);
        return HttpResponse::createNotFoundResponse(errorPage);
    }

    std::string content = fileReader.readFile(relativePath);
    std::string contentType = fileReader.getContentType(relativePath);

    return HttpResponse::createOkResponse(content, contentType);
}

HttpResponse Server::handlePostRequest(const HttpRequest& request,
                                        const RouteConfig* route)
{
    (void)request;
    (void)route;

    HttpResponse response;
    response.setStatus(501);
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>501 Not Implemented</h1>"
                     "<p>POST method not yet implemented</p>"
                     "</body></html>");
    return response;
}

HttpResponse Server::handleDeleteRequest(const HttpRequest& request,
                                          const RouteConfig* route)
{
    (void)request;
    (void)route;

    HttpResponse response;
    response.setStatus(501);
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>501 Not Implemented</h1>"
                     "<p>DELETE method not yet implemented</p>"
                     "</body></html>");
    return response;
}

HttpResponse Server::processRequest(const HttpRequest& request)
{
    std::string method = request.getMethod();
    std::string path = request.getPath();

    const RouteConfig* route = config.findRoute(path);

    if (route && !route->isMethodAllowed(method))
    {
        HttpResponse response;
        response.setStatus(405);
        response.setHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>405 Method Not Allowed</h1>"
                         "</body></html>");
        return response;
    }

    if (method == "GET")
        return handleGetRequest(request, route);
    else if (method == "POST")
        return handlePostRequest(request, route);
    else if (method == "DELETE")
        return handleDeleteRequest(request, route);
    else
    {
        HttpResponse response;
        response.setStatus(400);
        response.setHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>400 Bad Request</h1>"
                         "</body></html>");
        return response;
    }
}

void Server::handleClient(int clientFd)
{
    struct pollfd pollFd;
    pollFd.fd = clientFd;
    pollFd.events = POLLIN;

    int pollResult = poll(&pollFd, 1, 5000);
    if (pollResult < 0)
    {
        std::cerr << "Error: poll on client failed: "
                  << strerror(errno) << std::endl;
        return;
    }

    if (pollResult == 0)
    {
        std::cerr << "Error: client timeout" << std::endl;
        return;
    }

    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0)
    {
        std::cerr << "Error: recv failed: "
                  << strerror(errno) << std::endl;
        return;
    }

    if (bytesRead == 0)
    {
        std::cout << "Client disconnected" << std::endl;
        return;
    }

    std::string rawRequest(buffer, bytesRead);
    HttpRequest request;

    if (!request.parse(rawRequest))
    {
        std::cerr << "Error: failed to parse request" << std::endl;
        return;
    }

    std::cout << "Request: " << request.getMethod() << " "
              << request.getPath() << std::endl;

    HttpResponse response = processRequest(request);
    std::string responseStr = response.build();

    ssize_t bytesSent = send(clientFd, responseStr.c_str(),
                             responseStr.length(), 0);
    if (bytesSent < 0)
    {
        std::cerr << "Error: send failed: "
                  << strerror(errno) << std::endl;
    }
}

bool Server::start()
{
    if (!initSocket())
        return false;

    if (!bindAndListen())
    {
        stop();
        return false;
    }

    std::cout << "Server '" << config.getServerName()
              << "' listening on " << config.getHost()
              << ":" << config.getPort() << std::endl;
    return true;
}

void Server::run()
{
    while (true)
    {
        struct pollfd pollFd;
        pollFd.fd = serverFd;
        pollFd.events = POLLIN;

        int pollResult = poll(&pollFd, 1, -1);
        if (pollResult < 0)
        {
            std::cerr << "Error: poll failed: "
                      << strerror(errno) << std::endl;
            break;
        }

        if (pollFd.revents & POLLIN)
        {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientFd = accept(serverFd,
                                  (struct sockaddr*)&clientAddr,
                                  &clientAddrLen);
            if (clientFd < 0)
            {
                std::cerr << "Error: accept failed: "
                          << strerror(errno) << std::endl;
                continue;
            }

            std::cout << "Client connected" << std::endl;
            handleClient(clientFd);
            close(clientFd);
        }
    }
}

void Server::stop()
{
    if (serverFd >= 0)
    {
        close(serverFd);
        serverFd = -1;
    }
}
