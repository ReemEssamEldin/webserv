#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <string>

class FileReader
{
private:
    std::string rootDirectory;

public:
    FileReader();
    FileReader(const std::string& root);
    ~FileReader();

    void setRootDirectory(const std::string& root);
    bool fileExists(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    std::string getContentType(const std::string& path) const;
};

#endif
