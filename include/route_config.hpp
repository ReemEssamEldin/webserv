#ifndef ROUTE_CONFIG_HPP
#define ROUTE_CONFIG_HPP

#include <string>
#include <vector>
#include <map>

class RouteConfig
{
private:
    std::string path;
    std::vector<std::string> allowedMethods;
    std::string redirect;
    std::string root;
    bool autoindex;
    std::string indexFile;
    std::string uploadPath;
    std::map<std::string, std::string> cgiExtensions;

public:
    RouteConfig();
    RouteConfig(const std::string& routePath);
    ~RouteConfig();

    void setPath(const std::string& routePath);
    void addAllowedMethod(const std::string& method);
    void setRedirect(const std::string& redirectUrl);
    void setRoot(const std::string& rootPath);
    void setAutoindex(bool enabled);
    void setIndexFile(const std::string& file);
    void setUploadPath(const std::string& path);
    void addCgiExtension(const std::string& ext,
                         const std::string& interpreter);

    std::string getPath() const;
    bool isMethodAllowed(const std::string& method) const;
    std::string getRedirect() const;
    std::string getRoot() const;
    bool getAutoindex() const;
    std::string getIndexFile() const;
    std::string getUploadPath() const;
    std::string getCgiInterpreter(const std::string& ext) const;
    bool hasCgi(const std::string& ext) const;
};

#endif
