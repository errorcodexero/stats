#ifndef EVENT_ALLIANCES_H
#define EVENT_ALLIANCES_H

#include<vector>
#include "common.h"

class Alliance_count{
	int value;

	public:
	explicit Alliance_count(int);
	int get()const;
};

std::ostream& operator<<(std::ostream&,Alliance_count const&);

struct Event_alliances{
	Alliance_count count;

	struct Alliance{
		int number;//1-4 or 1-8
		Team_number captain,round1,round2;
		Maybe<Team_number> round3,backup;
		Maybe<Team_number> backupReplaced;
	};

	std::vector<Alliance> alliances;
};

std::ostream& operator<<(std::ostream&,Event_alliances::Alliance const&);
std::ostream& operator<<(std::ostream&,Event_alliances const&);

class Season{}; //only one value allowed for now

std::ostream& operator<<(std::ostream&,Season);

class Event_code{
	//this is normalized to lowercase, event though the API is case-insensitive
	std::string s;

	public:
	explicit Event_code(std::string);
	std::string const& get()const;
};

std::ostream& operator<<(std::ostream&,Event_code const&);

struct Alliance_query{
	Season season;
	Event_code event_code;
};

std::ostream& operator<<(std::ostream&,Alliance_query const&);

#endif
