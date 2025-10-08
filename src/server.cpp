#include "server.hpp"
#include "socket_utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <cerrno>

Server::Server() : serverFd(-1), port(8080), fileReader("www")
{
}

Server::Server(int portNumber) : serverFd(-1), port(portNumber),
                                  fileReader("www")
{
}

Server::~Server()
{
    stop();
}

void Server::setPort(int portNumber)
{
    port = portNumber;
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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

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

std::string Server::resolveFilePath(const std::string& requestPath) const
{
    if (requestPath == "/")
        return "/index.html";
    return requestPath;
}

HttpResponse Server::processRequest(const HttpRequest& request)
{
    std::string method = request.getMethod();
    std::string path = request.getPath();

    if (method != "GET")
    {
        HttpResponse response;
        response.setStatus(400);
        response.setBody("<html><body><h1>400 Bad Request</h1>"
                         "</body></html>");
        return response;
    }

    std::string filePath = resolveFilePath(path);

    if (!fileReader.fileExists(filePath))
    {
        std::string errorPage = fileReader.readFile("/errors/404.html");
        return HttpResponse::createNotFoundResponse(errorPage);
    }

    std::string content = fileReader.readFile(filePath);
    std::string contentType = fileReader.getContentType(filePath);

    return HttpResponse::createOkResponse(content, contentType);
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

    std::cout << "Server listening on port " << port << std::endl;
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
