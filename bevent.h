#ifndef BEVENT_H
#define BEVENT_H

#include<string>
#include "boost/date_time/gregorian/gregorian_types.hpp"
#include "maybe.h"

typedef boost::gregorian::date Date;

//Event info from the blue allaince.
struct BEvent{
	std::string name;
	Maybe<std::string> name_short;
	Maybe<Date> start_date;
	Maybe<Date> end_date;
	bool official;
	std::string key;
};

std::ostream& operator<<(std::ostream& o,BEvent const& b);

#endif
