#include "award_listings.h"
#include<sstream>

using namespace std;

/*#ifndef AWARD_LISTINGS_H
#define AWARD_LISTINGS_H

#define EVENT_TYPES \
	X(NONE)\
	X(REGIONAL)\
	X(DISTRICT_EVENT)\
	X(DISTRICT_CHAMPIONSHIP)\
	X(CHAMPIONSHIP_SUBDIVISION)\
	X(CHAMPIONSHIP_DIVISION)\
	X(CHAMPIONSHIP)

enum class Event_type{
	#define X(NAME) NAME,
	EVENT_TYPES
	#undef X
};
*/
ostream& operator<<(ostream& o,Event_type a){
	#define X(NAME) if(a==Event_type::NAME) return o<<""#NAME;
	EVENT_TYPES
	#undef X
	assert(0);
}
/*
#define AWARD_LISTING_ITEMS \
	X(int,awardId)\
	X(Event_type,eventType)\
	X(string,description)\
	X(bool,forPerson)

struct Award_listings{
	#define X(A,B) A B;
	AWARD_LISTING_ITEMS
	#undef X
};
*/
ostream& operator<<(ostream& o,Award_listings const& a){
	o<<"Award_listings(";
	#define X(A,B) o<<a.B<<" ";
	AWARD_LISTING_ITEMS
	#undef X
	return o<<")";
}

string award_query(Season season){
	stringstream ss;
	ss<<"/api/v1.0/awards/"<<season<<"/list";
	return ss.str();
}
