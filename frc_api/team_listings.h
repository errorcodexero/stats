#ifndef TEAM_LISTINGS_H
#define TEAM_LISTINGS_H

#include "common.h"
#include "event_listing.h"

typedef int Rookie_year;

#define TEAM_ITEMS\
	X(Team_number,teamNumber)\
	X(std::string,nameFull)\
	X(std::string,nameShort)\
	X(std::string,city)\
	X(std::string,stateprov)\
	X(std::string,country)\
	X(Rookie_year,rookieYear)\
	X(std::string,robotName)\
	X(Maybe<District_code>,districtCode)

struct Team{
	#define X(A,B) A B;
	TEAM_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Team const&);

#define TEAM_LISTINGS_ITEMS\
	X(int,pageCurrent)\
	X(int,pageTotal)\
	X(int,teamCountPage)\
	X(int,teamCountTotal)\
	X(std::vector<Team>,teams)

struct Team_listings{
	#define X(A,B) A B;
	TEAM_LISTINGS_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Team_listings const&);

struct Team_listings_query{
	Season season;
	At_most_one<Team_number,std::pair<Maybe<Event_code>,Maybe<District_code>>> restrictions;
	Maybe<int> page;
};

std::ostream& operator<<(std::ostream&,Team_listings_query const&);

#endif
