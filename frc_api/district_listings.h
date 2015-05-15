#ifndef DISTRICT_LISTINGS_H
#define DISTRICT_LISTINGS_H

#include "event_listing.h"

#define DISTRICT_ITEMS\
	X(District_code,code)\
	X(std::string,name)

struct District{
	#define X(A,B) A B;
	DISTRICT_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,District const&);

#define DISTRICT_LISTINGS_ITEMS\
	X(unsigned,districtCount)\
	X(std::vector<District>,districts)

struct District_listings{
	#define X(A,B) A B;
	DISTRICT_LISTINGS_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,District_listings const&);

struct District_query{
	Season season;
};

std::ostream& operator<<(std::ostream&,District_query);

#endif
