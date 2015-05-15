#ifndef EVENT_SCHEDULE_H
#define EVENT_SCHEDULE_H

#include "common.h"
#include "match_results.h"

#define SCHEDULE_ITEMS \
	X(std::string,description)\
	X(Tournament_level,level)\
	X(Match_number,matchNumber)\
	X(Datetime,startTime)\
	X(std::vector<Event_schedule::Team>,Teams)

#define SCHEDULE_TEAM_ITEMS \
	X(Team_number,teamNumber)\
	X(Alliance_station,station)\
	X(bool,surrogate)

struct Event_schedule{
	struct Team{
		#define X(A,B) A B;
		SCHEDULE_TEAM_ITEMS
		#undef X
	};
	#define X(A,B) A B;
	SCHEDULE_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Event_schedule::Team const&);
std::ostream& operator<<(std::ostream&,Event_schedule const&);

struct Event_schedule_query{
	Season season;
	Event_code event_code;
	At_least_one<Tournament_level,Team_number> data;
	Maybe<Match_number> start,end;
};

std::ostream& operator<<(std::ostream&,Event_schedule_query const&);

#endif
