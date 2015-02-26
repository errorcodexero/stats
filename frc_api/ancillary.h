#ifndef ANCILLARY_H
#define ANCILLARY_H

#include<string>

struct Ancillary{
	std::string name,version,status;
};
std::ostream& operator<<(std::ostream&,Ancillary const&);

extern const std::string ANCILLARY_QUERY;

#endif 
