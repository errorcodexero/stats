#ifndef STR_H
#define STR_H

#include<vector>
#include<string>

std::vector<std::string> split(std::string const&,char);
std::string toupper(std::string const&);
std::string tolower(std::string const&);
std::string lstrip(std::string const&);
std::string rstrip(std::string const&);
std::string strip(std::string const&);

#endif
