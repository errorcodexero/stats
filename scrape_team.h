#ifndef SCRAPE_TEAM_H
#define SCRAPE_TEAM_H

/*
Key 	Description 	Examples
website 	Official website associated with the team 	http://www.entech281.com
name 	Official long form name registered with FIRST 	Michelin/Caterpillar/Greenville Technical College/Greenville...
locality 	City of team derived from parsing the address registered with FIRST 	Greenville
region 	State of team derived from parsing the address registered with FIRST 	SC
country 	Country of team derived from parsing the address registered with FIRST 	USA
location 	Long form address that includes city, state, and country provided by FIRST 	Greenville, SC, USA
team_number 	Official team number issued by FIRST 	281
key 	TBA team key with the format frcyyyy 	frc281
nickname 	Team nickname provided by FIRST 	EnTech GreenVillians
events 	List of event models that the team attended for the year requested. 	
*/

#include<iosfwd>
#include<string>
#include "points.h"
#include "team.h"
#include "maybe.h"

struct Team_info{
	Maybe<std::string> website,name,locality,region;
	Team team;
	Maybe<std::string> country,location,team_number,key,nickname,events;

	explicit Team_info(Team);
};

std::ostream& operator<<(std::ostream&,Team_info const&);

void scrape_team_demo();

#endif
