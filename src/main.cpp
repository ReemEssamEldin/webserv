#include "server.hpp"
#include "config_parser.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Webserv starting..." << std::endl;

    std::string configFile = "conf/webserv.conf";

    if (argc > 1)
        configFile = argv[1];

    ConfigParser parser(configFile);
    if (!parser.parse())
    {
        std::cerr << "Error: failed to parse config file" << std::endl;
        return 1;
    }

    if (parser.getServerCount() == 0)
    {
        std::cerr << "Error: no servers configured" << std::endl;
        return 1;
    }

    Server server(parser.getServer(0));

    if (!server.start())
    {
        std::cerr << "Error: failed to start server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}
