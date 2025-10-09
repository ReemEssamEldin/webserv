#include "config_parser.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

ConfigParser::ConfigParser() : configFile("")
{
}

ConfigParser::ConfigParser(const std::string& file) : configFile(file)
{
}

ConfigParser::~ConfigParser()
{
}

std::string ConfigParser::trim(const std::string& str) const
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> ConfigParser::split(const std::string& str,
                                             char delimiter) const
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);

    while (std::getline(stream, token, delimiter))
    {
        std::string trimmed = trim(token);
        if (!trimmed.empty())
            tokens.push_back(trimmed);
    }

    return tokens;
}

size_t ConfigParser::parseSize(const std::string& value) const
{
    std::string numStr = value;
    size_t multiplier = 1;

    if (!value.empty())
    {
        char lastChar = value[value.length() - 1];
        if (lastChar == 'M' || lastChar == 'm')
        {
            multiplier = 1024 * 1024;
            numStr = value.substr(0, value.length() - 1);
        }
        else if (lastChar == 'K' || lastChar == 'k')
        {
            multiplier = 1024;
            numStr = value.substr(0, value.length() - 1);
        }
    }

    return std::atoi(numStr.c_str()) * multiplier;
}

void ConfigParser::parseLocationDirective(const std::string& line,
                                          RouteConfig& route)
{
    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.empty())
        return;

    std::string directive = tokens[0];

    if (directive == "allow_methods" && tokens.size() > 1)
    {
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            std::string method = tokens[i];
            if (method[method.length() - 1] == ';')
                method = method.substr(0, method.length() - 1);
            route.addAllowedMethod(method);
        }
    }
    else if (directive == "return" && tokens.size() > 1)
    {
        std::string redirect = tokens[1];
        if (redirect[redirect.length() - 1] == ';')
            redirect = redirect.substr(0, redirect.length() - 1);
        route.setRedirect(redirect);
    }
    else if (directive == "root" && tokens.size() > 1)
    {
        std::string root = tokens[1];
        if (root[root.length() - 1] == ';')
            root = root.substr(0, root.length() - 1);
        route.setRoot(root);
    }
    else if (directive == "autoindex" && tokens.size() > 1)
    {
        std::string value = tokens[1];
        if (value[value.length() - 1] == ';')
            value = value.substr(0, value.length() - 1);
        route.setAutoindex(value == "on");
    }
    else if (directive == "index" && tokens.size() > 1)
    {
        std::string index = tokens[1];
        if (index[index.length() - 1] == ';')
            index = index.substr(0, index.length() - 1);
        route.setIndexFile(index);
    }
    else if (directive == "upload_path" && tokens.size() > 1)
    {
        std::string path = tokens[1];
        if (path[path.length() - 1] == ';')
            path = path.substr(0, path.length() - 1);
        route.setUploadPath(path);
    }
    else if (directive == "cgi_extension" && tokens.size() > 2)
    {
        std::string ext = tokens[1];
        std::string interpreter = tokens[2];
        if (interpreter[interpreter.length() - 1] == ';')
            interpreter = interpreter.substr(0, interpreter.length() - 1);
        route.addCgiExtension(ext, interpreter);
    }
}

void ConfigParser::parseServerDirective(const std::string& line,
                                        ServerConfig& config)
{
    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.empty())
        return;

    std::string directive = tokens[0];

    if (directive == "listen" && tokens.size() > 1)
    {
        std::string listenValue = tokens[1];
        if (listenValue[listenValue.length() - 1] == ';')
            listenValue = listenValue.substr(0, listenValue.length() - 1);

        size_t colonPos = listenValue.find(':');
        if (colonPos != std::string::npos)
        {
            config.setHost(listenValue.substr(0, colonPos));
            config.setPort(std::atoi(
                listenValue.substr(colonPos + 1).c_str()));
        }
        else
        {
            config.setPort(std::atoi(listenValue.c_str()));
        }
    }
    else if (directive == "server_name" && tokens.size() > 1)
    {
        std::string name = tokens[1];
        if (name[name.length() - 1] == ';')
            name = name.substr(0, name.length() - 1);
        config.setServerName(name);
    }
    else if (directive == "client_max_body_size" && tokens.size() > 1)
    {
        std::string size = tokens[1];
        if (size[size.length() - 1] == ';')
            size = size.substr(0, size.length() - 1);
        config.setClientMaxBodySize(parseSize(size));
    }
    else if (directive == "error_page" && tokens.size() > 2)
    {
        int code = std::atoi(tokens[1].c_str());
        std::string path = tokens[2];
        if (path[path.length() - 1] == ';')
            path = path.substr(0, path.length() - 1);
        config.setErrorPage(code, path);
    }
    else if (directive == "root" && tokens.size() > 1)
    {
        std::string root = tokens[1];
        if (root[root.length() - 1] == ';')
            root = root.substr(0, root.length() - 1);
        config.setDefaultRoot(root);
    }
}

bool ConfigParser::parseLocationBlock(std::ifstream& file,
                                      RouteConfig& route)
{
    std::string line;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line == "}")
            return true;

        parseLocationDirective(line, route);
    }

    return false;
}

bool ConfigParser::parseServerBlock(std::ifstream& file,
                                    ServerConfig& config)
{
    std::string line;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line == "}")
            return true;

        if (line.find("location") == 0)
        {
            std::vector<std::string> tokens = split(line, ' ');
            if (tokens.size() > 1)
            {
                std::string path = tokens[1];
                if (path[path.length() - 1] == '{')
                    path = path.substr(0, path.length() - 1);
                path = trim(path);

                RouteConfig route(path);
                if (parseLocationBlock(file, route))
                    config.addRoute(route);
            }
        }
        else
        {
            parseServerDirective(line, config);
        }
    }

    return false;
}

bool ConfigParser::parse()
{
    return parse(configFile);
}

bool ConfigParser::parse(const std::string& file)
{
    std::ifstream configStream(file.c_str());
    if (!configStream.is_open())
    {
        std::cerr << "Error: cannot open config file: "
                  << file << std::endl;
        return false;
    }

    servers.clear();
    std::string line;

    while (std::getline(configStream, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line == "server {")
        {
            ServerConfig config;
            if (parseServerBlock(configStream, config))
                servers.push_back(config);
        }
    }

    configStream.close();

    if (servers.empty())
    {
        std::cerr << "Error: no server blocks found in config"
                  << std::endl;
        return false;
    }

    return true;
}

const std::vector<ServerConfig>& ConfigParser::getServers() const
{
    return servers;
}

size_t ConfigParser::getServerCount() const
{
    return servers.size();
}

const ServerConfig& ConfigParser::getServer(size_t index) const
{
    return servers[index];
}
