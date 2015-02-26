#ifndef AWARD_LISTINGS_H
#define AWARD_LISTINGS_H

#include "event_alliances.h"

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

std::ostream& operator<<(std::ostream&,Event_type);

#define AWARD_LISTING_ITEMS \
	X(int,awardId)\
	X(Event_type,eventType)\
	X(std::string,description)\
	X(bool,forPerson)

struct Award_listings{
	#define X(A,B) A B;
	AWARD_LISTING_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Award_listings const&);

std::string award_query(Season);

#endif
