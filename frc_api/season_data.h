#ifndef SEASON_DATA_H
#define SEASON_DATA_H

#include "match_results.h"

#define SEASON_DATA_ITEMS\
	X(Datetime,frcChampionship)\
	X(unsigned,eventCount)\
	X(std::string,gameName)\
	X(Datetime,kickoff)\
	X(Team_number,rookieStart)\
	X(unsigned,teamCount)

struct Season_data{
	#define X(A,B) A B;
	SEASON_DATA_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream& o,Season_data const&);

struct Season_data_query{
	Season season;
};

std::ostream& operator<<(std::ostream&,Season_data_query);

#endif
