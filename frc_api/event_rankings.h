#ifndef EVENT_RANKINGS_H
#define EVENT_RANKINGS_H

#include "event_alliances.h"

//qualification average should really by a decimal type, not floating point
#define EVENT_RANKINGS_ITEMS \
	X(unsigned,autoPoints)\
	X(unsigned,containerPoints)\
	X(unsigned,coopertitionPoints)\
	X(unsigned,dq)\
	X(unsigned,litterPoints)\
	X(unsigned,matchesPlayed)\
	X(double,qualAverage)\
	X(Positive_int,rank)\
	X(Team_number,teamNumber)\
	X(unsigned,totePoints)\
	X(unsigned,wins)\
	X(unsigned,losses)\
	X(unsigned,ties)

struct Event_rankings{
	#define X(A,B) A B;
	EVENT_RANKINGS_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Event_rankings const&);

struct Event_rankings_query{
	Season season;
	Event_code event_code;
	Maybe<Team_number> teamNumber;
	bool top;
};

std::ostream& operator<<(std::ostream&,Event_rankings_query const&);

#endif
