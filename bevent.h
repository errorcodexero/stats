#ifndef BEVENT_H
#define BEVENT_H

#include<string>
#include "maybe.h"

//Event info from the blue allaince.
struct BEvent{
	std::string name;
	Maybe<std::string> name_short;
	Maybe<std::string> start_date;
	Maybe<std::string> end_date;
	bool official;
	std::string key;
};

std::ostream& operator<<(std::ostream& o,BEvent const& b);

#endif
