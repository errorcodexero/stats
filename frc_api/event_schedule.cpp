#include "event_schedule.h"
#include "../util.h"

using namespace std;

ostream& operator<<(ostream& o,Event_schedule::Team const& a){
	o<<"Es::Team(";
	#define X(A,B) o<<a.B;
	SCHEDULE_TEAM_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_schedule const& a){
	o<<"Event_schedule( ";
	#define X(A,B) o<<a.B<<" ";
	SCHEDULE_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_schedule_query const& a){
	o<<"/api/v1.0/schedule/"<<a.season<<"/"<<a.event_code<<"/";
	Add_optional opt(o);
	opt("tournamentLevel",a.data.a());
	opt("teamNumber",a.data.b());
	opt("start",a.start);
	opt("end",a.end);
	return o;
}
