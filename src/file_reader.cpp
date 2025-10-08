#include "file_reader.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

FileReader::FileReader() : rootDirectory("www")
{
}

FileReader::FileReader(const std::string& root) : rootDirectory(root)
{
}

FileReader::~FileReader()
{
}

void FileReader::setRootDirectory(const std::string& root)
{
    rootDirectory = root;
}

bool FileReader::fileExists(const std::string& path) const
{
    struct stat buffer;
    std::string fullPath = rootDirectory + path;
    return (stat(fullPath.c_str(), &buffer) == 0);
}

std::string FileReader::readFile(const std::string& path) const
{
    std::string fullPath = rootDirectory + path;
    std::ifstream file(fullPath.c_str());

    if (!file.is_open())
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

std::string FileReader::getContentType(const std::string& path) const
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos)
        return "text/plain";

    std::string extension = path.substr(dotPos + 1);

    if (extension == "html" || extension == "htm")
        return "text/html";
    else if (extension == "css")
        return "text/css";
    else if (extension == "js")
        return "application/javascript";
    else if (extension == "json")
        return "application/json";
    else if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    else if (extension == "png")
        return "image/png";
    else if (extension == "gif")
        return "image/gif";
    else if (extension == "txt")
        return "text/plain";
    else
        return "application/octet-stream";
}
