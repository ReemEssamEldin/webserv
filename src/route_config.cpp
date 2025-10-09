#include "route_config.hpp"
#include <algorithm>

RouteConfig::RouteConfig() : path("/"), autoindex(false),
                             indexFile("index.html")
{
}

RouteConfig::RouteConfig(const std::string& routePath)
    : path(routePath), autoindex(false), indexFile("index.html")
{
}

RouteConfig::~RouteConfig()
{
}

void RouteConfig::setPath(const std::string& routePath)
{
    path = routePath;
}

void RouteConfig::addAllowedMethod(const std::string& method)
{
    allowedMethods.push_back(method);
}

void RouteConfig::setRedirect(const std::string& redirectUrl)
{
    redirect = redirectUrl;
}

void RouteConfig::setRoot(const std::string& rootPath)
{
    root = rootPath;
}

void RouteConfig::setAutoindex(bool enabled)
{
    autoindex = enabled;
}

void RouteConfig::setIndexFile(const std::string& file)
{
    indexFile = file;
}

void RouteConfig::setUploadPath(const std::string& path)
{
    uploadPath = path;
}

void RouteConfig::addCgiExtension(const std::string& ext,
                                  const std::string& interpreter)
{
    cgiExtensions[ext] = interpreter;
}

std::string RouteConfig::getPath() const
{
    return path;
}

bool RouteConfig::isMethodAllowed(const std::string& method) const
{
    if (allowedMethods.empty())
        return true;

    std::vector<std::string>::const_iterator it;
    for (it = allowedMethods.begin(); it != allowedMethods.end(); ++it)
    {
        if (*it == method)
            return true;
    }
    return false;
}

std::string RouteConfig::getRedirect() const
{
    return redirect;
}

std::string RouteConfig::getRoot() const
{
    return root;
}

bool RouteConfig::getAutoindex() const
{
    return autoindex;
}

std::string RouteConfig::getIndexFile() const
{
    return indexFile;
}

std::string RouteConfig::getUploadPath() const
{
    return uploadPath;
}

std::string RouteConfig::getCgiInterpreter(const std::string& ext) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = cgiExtensions.find(ext);
    if (it != cgiExtensions.end())
        return it->second;
    return "";
}

bool RouteConfig::hasCgi(const std::string& ext) const
{
    return cgiExtensions.find(ext) != cgiExtensions.end();
}
