#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "server_config.hpp"
#include <string>
#include <vector>
#include <fstream>

class ConfigParser
{
private:
    std::vector<ServerConfig> servers;
    std::string configFile;

    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str,
                                   char delimiter) const;
    bool parseServerBlock(std::ifstream& file, ServerConfig& config);
    bool parseLocationBlock(std::ifstream& file, RouteConfig& route);
    void parseServerDirective(const std::string& line,
                               ServerConfig& config);
    void parseLocationDirective(const std::string& line,
                                RouteConfig& route);
    size_t parseSize(const std::string& value) const;

public:
    ConfigParser();
    ConfigParser(const std::string& file);
    ~ConfigParser();

    bool parse();
    bool parse(const std::string& file);
    const std::vector<ServerConfig>& getServers() const;
    size_t getServerCount() const;
    const ServerConfig& getServer(size_t index) const;
};

#endif
