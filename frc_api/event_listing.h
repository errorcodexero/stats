#ifndef EVENT_LISTING_H
#define EVENT_LISTING_H

#include "event_alliances.h"
#include "award_listings.h"
#include "match_results.h"

typedef Event_code District_code;//they follow the same format, except numbers not allowed in district codes

//Event_type may need to be different for this than it was for awards, None is not a value given here
#define EVENT_ITEMS\
	X(Event_code,code)\
	X(Maybe<Event_code>,divisionCode)\
	X(std::string,name)\
	X(Event_type,type)\
	X(Maybe<District_code>,districtCode)\
	X(std::string,venue)\
	X(std::string,city)\
	X(std::string,stateProv)\
	X(std::string,country)\
	X(Datetime,dateStart)\
	X(Datetime,dateEnd)

struct Event{
	#define X(A,B) A B;
	EVENT_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Event const&);

#define EVENT_LISTING_ITEMS\
	X(std::vector<Event>,events)\
	X(unsigned,eventCount)

struct Event_listing{
	#define X(A,B) A B;
	EVENT_LISTING_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Event_listing const&);

class Event_listing_query{
	public:
	struct Data{
		Season season;
		Maybe<Event_code> eventCode;
		Maybe<Team_number> teamNumber;
		Maybe<District_code> districtCode;
		Maybe<bool> excludeDistrict;
	};

	private:
	Data data_;

	public:
	explicit Event_listing_query(Data);
	Data get()const;
};

std::ostream& operator<<(std::ostream&,Event_listing_query::Data const&);
std::ostream& operator<<(std::ostream&,Event_listing_query const&);

#endif
