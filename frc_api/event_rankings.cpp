#include "event_rankings.h"
#include "../util.h"
#include "match_results.h"

using namespace std;

ostream& operator<<(ostream& o,Event_rankings const& a){
	o<<"Event_rankings(";
	#define X(A,B) o<<" "<<a.B;
	EVENT_RANKINGS_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_rankings_query const& a){
	o<<"/api/v1.0/rankings/"<<a.season<<"/"<<a.event_code;
	Add_optional opt(o);
	opt("teamNumber",a.teamNumber);
	if(a.top) opt("top");
	return o;
}
