#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

std::string BaseName( std::string const& fullpath );
std::string DirName( std::string const& fullpath );
std::string ExtName( std::string const& fullpath );
std::string ToLower( std::string const& s );
void SplitLine( std::string const& line, std::vector<std::string>& args );

#endif // UTIL_H

