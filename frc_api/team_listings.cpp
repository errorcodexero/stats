#include "team_listings.h"
#include "../util.h"

using namespace std;

ostream& operator<<(ostream& o,Team const& a){
	o<<"Team(";
	#define X(A,B) o<<a.B<<" ";
	TEAM_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Team_listings const& a){
	o<<"Team_listings(";
	#define X(A,B) o<<a.B<<" ";
	TEAM_LISTINGS_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Team_listings_query const& a){
	o<<"/api/v1.0/teams/"<<a.season;
	Add_optional opt(o);
	opt("teamNumber",a.restrictions.a());
	if(a.restrictions.b()){
		auto x=*a.restrictions.b();
		opt("eventCode",x.first);
		opt("districtCode",x.second);
	}
	opt("page",a.page);
	return o;
}
