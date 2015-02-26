#include "hybrid_schedule.h"
#include "../util.h"

using namespace std;

ostream& operator<<(ostream& o,Hybrid_schedule::Team const& a){
	o<<"Hybrid_schedule_team(";
	#define X(A,B) o<<a.B<<" ";
	HYBRID_SCHEDULE_TEAM_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Hybrid_schedule const& a){
	o<<"Hybrid_schedule(";
	#define X(A,B) o<<a.B<<" ";
	HYBRID_SCHEDULE_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Hybrid_schedule_query const& a){
	o<<"/api/v1.0/schedule/"<<a.season<<"/"<<a.eventCode<<"/"<<a.tournamentLevel<<"/hybrid";
	Add_optional opt(o);
	opt("start",a.start);
	opt("end",a.end);
	return o<<")";
}
