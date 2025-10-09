#ifndef DIRECTORY_LISTING_HPP
#define DIRECTORY_LISTING_HPP

#include <string>

class DirectoryListing
{
private:
    std::string buildListingHtml(const std::string& path,
                                  const std::string& requestPath) const;

public:
    DirectoryListing();
    ~DirectoryListing();

    std::string generateListing(const std::string& path,
                                 const std::string& requestPath) const;
};

#endif
