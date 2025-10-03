#include "socket_utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main()
{
    std::cout << "Webserv starting..." << std::endl;

    int serverFd;
    int clientFd;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    const char* message = "Hello from Webserv!\n";
    int port = 8080;

    // Create socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        std::cerr << "Error: socket creation failed: "
                  << strerror(errno) << std::endl;
        return 1;
    }

    // Set socket to non-blocking
    if (setNonBlocking(serverFd) < 0)
    {
        std::cerr << "Error: failed to set non-blocking: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        return 1;
    }

    // Allow socket reuse (cast for cross-platform compatibility)
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR,
                   (const void*)&opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error: setsockopt failed: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        return 1;
    }

    // Prepare server address
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind socket
    if (bind(serverFd, (struct sockaddr*)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error: bind failed: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        return 1;
    }

    // Listen for connections
    if (listen(serverFd, 10) < 0)
    {
        std::cerr << "Error: listen failed: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;
    std::cout << "Waiting for connection..." << std::endl;

    // Accept one connection
    clientAddrLen = sizeof(clientAddr);
    clientFd = accept(serverFd, (struct sockaddr*)&clientAddr,
                      &clientAddrLen);
    if (clientFd < 0)
    {
        std::cerr << "Error: accept failed: "
                  << strerror(errno) << std::endl;
        close(serverFd);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    // Send message to client
    ssize_t bytesSent = send(clientFd, message, strlen(message), 0);
    if (bytesSent < 0)
    {
        std::cerr << "Error: send failed: "
                  << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Message sent to client" << std::endl;
    }

    // Close connections
    close(clientFd);
    close(serverFd);

    std::cout << "Connection closed. Server shutting down." << std::endl;

    return 0;
}
