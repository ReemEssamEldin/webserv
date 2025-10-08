#include "server.hpp"
#include <iostream>

int main()
{
    std::cout << "Webserv starting..." << std::endl;

    Server server(8080);

    if (!server.start())
    {
        std::cerr << "Error: failed to start server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}
