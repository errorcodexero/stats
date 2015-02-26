#ifndef EVENT_AWARDS_H
#define EVENT_AWARDS_H

#include<string>
#include "../maybe.h"
#include "common.h"
#include "event_alliances.h"

#define EVENT_AWARD_ITEMS \
	X(int,awardId)\
	X(int,teamId)\
	X(int,eventId)\
	X(int,eventDivisionId)\
	X(std::string,name)\
	X(int,series)\
	X(Maybe<Team_number>,teamNumber)\
	X(Maybe<std::string>,person)

struct Event_awards{
	#define X(a,b) a b;
	EVENT_AWARD_ITEMS
	#undef X

	Event_awards();
};

std::ostream& operator<<(std::ostream&,Event_awards const&);

struct Event_awards_query{
	Season season;
	At_least_one<Event_code,Team_number> data;
};

std::ostream& operator<<(std::ostream&,Event_awards_query const&);

#endif
