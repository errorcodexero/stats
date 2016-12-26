#ifndef TEAM_LIST_H
#define TEAM_LIST_H

#include<vector>
#include<string>
#include<map>
#include "../maybe.h"

#define TEAM_INFO_ITEMS(X)\
	X(Maybe<std::string>,website)\
	X(Maybe<std::string>,name)\
	X(Maybe<std::string>,locality)\
	X(Maybe<std::string>,region)\
	X(Maybe<std::string>,country_name)\
	X(Maybe<std::string>,location)\
	X(int,team_number)\
	X(std::string,key)\
	X(Maybe<std::string>,nickname)\
	X(Maybe<int>,rookie_year)\
	X(Maybe<std::string>,motto)

struct Team_info{
	#define X(A,B) A B;
	TEAM_INFO_ITEMS(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Team_info const&);

std::vector<Team_info> all_team_details();
std::vector<Team_info> const& event_teams(std::string const& event_key);

using Year=int;
std::map<Year,std::string> team_history_districts(std::string const& team_key);

#endif
