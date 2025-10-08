#ifndef SERVER_HPP
#define SERVER_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include "file_reader.hpp"
#include <string>

class Server
{
private:
    int serverFd;
    int port;
    FileReader fileReader;

    bool initSocket();
    bool bindAndListen();
    void handleClient(int clientFd);
    HttpResponse processRequest(const HttpRequest& request);
    std::string resolveFilePath(const std::string& requestPath) const;

public:
    Server();
    Server(int portNumber);
    ~Server();

    void setPort(int portNumber);
    bool start();
    void run();
    void stop();
};

#endif
