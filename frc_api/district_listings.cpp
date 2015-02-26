#include "district_listings.h"
#include "../util.h"

using namespace std;

ostream& operator<<(ostream& o,District const& a){
	o<<"District(";
	#define X(A,B) o<<a.B<<" ";
	DISTRICT_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,District_listings const& a){
	o<<"District_listings(";
	#define X(A,B) o<<a.B<<" ";
	DISTRICT_LISTINGS_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,District_query a){
	return o<<"/api/v1.0/districts/"<<a.season;
}
