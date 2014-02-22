#ifndef SCRAPE_H
#define SCRAPE_H

#include "match_info.h"
#include "bevent.h"
#include "team.h"

std::vector<std::string> get_event_keys(int);
std::vector<Match_info> matches(int);
std::vector<BEvent> get_events(int year);

//data from the event details page of the blue alliance
struct BEvent_details{
	Maybe<std::string> location;
	std::string key;
	std::string year;//could actually make this an int
	Maybe<std::string> start_date;
	std::string name;
	std::vector<Team> teams;
	bool official;
	std::vector<std::string> matches;
	std::string event_code;
	Maybe<std::string> end_date;
	Maybe<std::string> name_short;
	Maybe<bool> facebook_eid; //fix the type of this.
};
std::ostream& operator<<(std::ostream&,BEvent_details const&);

BEvent_details get_details(std::string const& event_code);

#endif
