#include "directory_listing.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <algorithm>

DirectoryListing::DirectoryListing()
{
}

DirectoryListing::~DirectoryListing()
{
}

std::string DirectoryListing::buildListingHtml(
    const std::string& path,
    const std::string& requestPath) const
{
    std::ostringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<title>Index of " << requestPath << "</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; }\n";
    html << "h1 { color: #333; }\n";
    html << "table { border-collapse: collapse; width: 100%; }\n";
    html << "th, td { text-align: left; padding: 8px; }\n";
    html << "th { background-color: #f2f2f2; border-bottom: 2px "
         << "solid #ddd; }\n";
    html << "tr:hover { background-color: #f5f5f5; }\n";
    html << "a { color: #0066cc; text-decoration: none; }\n";
    html << "a:hover { text-decoration: underline; }\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";
    html << "<h1>Index of " << requestPath << "</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Name</th><th>Type</th></tr>\n";

    DIR* dir = opendir(path.c_str());
    if (!dir)
        return "";

    std::vector<std::string> files;
    std::vector<std::string> dirs;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        if (name == ".")
            continue;

        std::string fullPath = path + "/" + name;
        struct stat fileStat;

        if (stat(fullPath.c_str(), &fileStat) < 0)
            continue;

        if (S_ISDIR(fileStat.st_mode))
            dirs.push_back(name);
        else
            files.push_back(name);
    }

    closedir(dir);

    std::sort(dirs.begin(), dirs.end());
    std::sort(files.begin(), files.end());

    for (size_t i = 0; i < dirs.size(); ++i)
    {
        std::string link = requestPath;
        if (link[link.length() - 1] != '/')
            link += "/";
        link += dirs[i];

        html << "<tr><td><a href=\"" << link << "/\">"
             << dirs[i] << "/</a></td><td>Directory</td></tr>\n";
    }

    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string link = requestPath;
        if (link[link.length() - 1] != '/')
            link += "/";
        link += files[i];

        html << "<tr><td><a href=\"" << link << "\">"
             << files[i] << "</a></td><td>File</td></tr>\n";
    }

    html << "</table>\n";
    html << "</body>\n</html>\n";

    return html.str();
}

std::string DirectoryListing::generateListing(
    const std::string& path,
    const std::string& requestPath) const
{
    return buildListingHtml(path, requestPath);
}
