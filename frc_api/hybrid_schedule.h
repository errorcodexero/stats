#ifndef HYBRID_SCHEDULE_H
#define HYBRID_SCHDEULE_H

#include "common.h"
#include "match_results.h"

#define HYBRID_SCHEDULE_ITEMS\
	X(std::string,description)\
	X(Tournament_level,tournamentLevel)\
	X(Match_number,matchNumber)\
	X(Datetime,startTime)\
	X(unsigned,scoreRedFinal)\
	X(unsigned,scoreRedFoul)\
	X(unsigned,scoreRedAuto)\
	X(unsigned,scoreBlueFinal)\
	X(unsigned,scoreBlueFoul)\
	X(unsigned,scoreBlueAuto)\
	X(std::vector<Hybrid_schedule::Team>,Teams)

#define HYBRID_SCHEDULE_TEAM_ITEMS\
	X(Team_number,teamNumber)\
	X(Alliance_station,station)\
	X(bool,surrogate)\
	X(bool,dq)

struct Hybrid_schedule{
	struct Team{
		#define X(A,B) A B;
		HYBRID_SCHEDULE_TEAM_ITEMS
		#undef X
	};

	#define X(A,B) A B;
	HYBRID_SCHEDULE_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Hybrid_schedule::Team const&);
std::ostream& operator<<(std::ostream&,Hybrid_schedule const&);

struct Hybrid_schedule_query{
	Season season;
	Event_code eventCode;
	Tournament_level tournamentLevel;
	Maybe<Match_number> start,end;
};

std::ostream& operator<<(std::ostream&,Hybrid_schedule_query const&);

#endif
