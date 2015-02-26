#include "event_listing.h"

using namespace std;

ostream& operator<<(ostream& o,Event const& a){
	o<<"Event(";
	#define X(A,B) o<<a.B<<" ";
	EVENT_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_listing const& a){
	o<<"Event_listings(";
	#define X(A,B) o<<a.B<<" ";
	EVENT_LISTING_ITEMS
	#undef X
	return o<<")";
}

Event_listing_query::Event_listing_query(Data a):data_(a){
	if(a.eventCode){
		assert(!a.teamNumber && !a.districtCode && !a.excludeDistrict);
	}
	if(a.districtCode){
		assert(!a.eventCode && !a.excludeDistrict);
	}
}

Data Event_listing_query::get()const{ return data_; }

ostream& operator<<(ostream& o,Event_listing_query const& a){
	o<<"/api/v1.0/events/"<<a.get().season;
	Add_optional opt(o);
	opt("eventCode",a.get().eventCode);
	opt("teamNumber",a.get().teamNumber);
	opt("districtCode",a.get().districtCode);
	opt("excludeDistrict",a.get().excludeDistrict);
	return o;
}
