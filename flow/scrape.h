#ifndef SCRAPE_H
#define SCRAPE_H

#include<vector>
#include "../maybe.h"

typedef int Year;

std::vector<Year> years();

struct Team{
	int num;
	char modifier;

	explicit Team(int);
	explicit Team(std::string);
};

std::ostream& operator<<(std::ostream&,Team);

bool operator<(Team,Team);
bool operator==(Team,Team);

struct Alliance{
	std::vector<Team> declines;
	std::vector<Team> picks;
};

std::ostream& operator<<(std::ostream&,Alliance const&);

bool operator<(Alliance const&,Alliance const&);
bool operator==(Alliance const&,Alliance const&);

struct Webcast{
	std::string channel,type;
	Maybe<std::string> file;
};

std::ostream& operator<<(std::ostream&,Webcast const&);

bool operator<(Webcast const&,Webcast const&);
bool operator==(Webcast const&,Webcast const&);

#define EVENT_ITEMS \
	X(std::vector<Alliance>,alliances)\
	X(Maybe<std::string>,end_date)\
	X(std::string,event_code)\
	X(Maybe<int>,event_district)\
	X(Maybe<std::string>,event_district_string)\
	X(int,event_type)\
	X(std::string,event_type_string)\
	X(Maybe<std::string>,facebook_eid)\
	X(std::string,key)\
	X(Maybe<std::string>,location)\
	X(std::string,name)\
	X(bool,official)\
	X(Maybe<std::string>,short_name)\
	X(Maybe<std::string>,start_date)\
	X(Maybe<std::string>,venue_address)\
	X(std::vector<Webcast>,webcast)\
	X(Maybe<std::string>,website)\
	X(Year,year)

struct Event{
	#define X(A,B) A B;
	EVENT_ITEMS
	#undef X
};

std::ostream& operator<<(std::ostream&,Event const&);

bool operator<(Event const&,Event const&);
bool operator==(Event const&,Event const&);
bool operator<=(Event const&,Event const&);

std::vector<Event> events(Year);

#define TEAM_INFO_ITEMS\
	X(website,Maybe<std::string>)\
	X(name,Maybe<std::string>)\
	X(locality,Maybe<std::string>)\
	X(rookie_year,Maybe<int>)\
	X(region,Maybe<std::string>)\
	X(team_number,int)\
	X(location,Maybe<std::string>)\
	X(key,std::string)\
	X(country_name,Maybe<std::string>)\
	X(nickname,Maybe<std::string>)

struct Team_info{
	#define X(A,B) B A;
	TEAM_INFO_ITEMS
	#undef X
};
std::ostream& operator<<(std::ostream& o,Team_info const&);

std::vector<Team_info> teams_at_event(std::string event_key);
std::vector<Team_info> teams();

std::string key(Event e);
std::vector<std::string> event_keys(std::vector<Event>);

struct District{
	std::string name,key;
};
std::ostream& operator<<(std::ostream& o,District const&);

std::vector<District> districts(Year);
std::vector<Event> district_events(Year,std::string district_key);

Team_info team_info(Team team);
std::vector<Event> team_events(Team,Year);

struct Recipient{
	Maybe<Team> team;
	Maybe<std::string> awardee;
};
std::ostream& operator<<(std::ostream&,Recipient const&);

#define AWARD_TYPES \
	X(CHAIRMANS,0)\
	X(WINNER,1)\
	X(FINALIST,2)\
	X(WOODIE_FLOWERS,3)\
	X(DEANS_LIST,4)\
	X(VOLUNTEER,5)\
	X(FOUNDERS,6)\
	X(BART_KAMEN_MEMORIAL,7)\
	X(MAKE_IT_LOUD,8)\
	X(ENGINEERING_INSPIRATION,9)\
	X(ROOKIE_ALL_STAR,10)\
	X(GRACIOUS_PROFESSIONALISM,11)\
	X(COOPERTITION,12)\
	X(JUDGES,13)\
	X(HIGHEST_ROOKIE_SEED,14)\
	X(ROOKIE_INSPIRATION,15)\
	X(INDUSTRIAL_DEESIGN,16)\
	X(QUALITY,17)\
	X(SAFETY,18)\
	X(SPORTSMANSHIP,19)\
	X(CREATIVITY,20)\
	X(ENGINEERING_EXCELLENCE,21)\
	X(ENTREPRENEURSHIP,22)\
	X(EXCELLENCE_IN_DESIGN,23)\
	X(EXCELLENCE_IN_DESIGN_CAD,24)\
	X(EXCELLENCE_IN_DESIGN_ANIMATION,25)\
	X(DRIVING_TOMORROWS_TECHNOLOGY,26)\
	X(IMAGERY,27)\
	X(MEDIA_AND_TECHNOLOGY,28)\
	X(INNOVATION_IN_CONTROL,29)\
	X(SPIRIT,30)\
	X(WEBSITE,31)\
	X(VISUALIZATION,32)\
	X(AUTODESK_INVENTOR,33)\
	X(FUTURE_INNOVATOR,34)\
	X(RECOGNITION_OF_EXTRAORDINARY_SERVICE,35)\
	X(OUTSTANDING_CART,36)\
	X(WSU_AIM_HIGHER,37)\
	X(LEADERSHIP_IN_CONTROL,38)\
	X(NUM_1_SEED,39)\
	X(INCREDIBLE_PLAY,40)\
	X(PEOPLES_CHOICE_ANIMATION,41)\
	X(VISUALIZATION_RISING_STAR,42)\
	X(BEST_OFFENSIVE_ROUND,43)\
	X(BEST_PLAY_OF_THE_DAY,44)\
	X(FEATHERWEIGHT_IN_THE_FINALS,45)\
	X(MOST_PHOTOGENIC,46)\
	X(OUTSTANDING_DEFENSE,47)\
	X(POWER_TO_SIMPLIFY,48)\
	X(AGAINST_ALL_ODDS,49)\
	X(RISING_STAR,50)\
	X(CHAIRMANS_HONORABLE_MENTION,51)\
	X(CONTENT_COMMUNICATION_HONORABLE_MENTION,52)\
	X(TECHNICAL_EXECUTION_HONORABLE_MENTION,53)\
	X(REALIZATION,54)\
	X(REALIZATION_HONORABLE_MENTION,55)\
	X(DESIGN_YOUR_FUTURE,56)\
	X(DESIGN_YOUR_FUTURE_HONORABLE_MENTION,57)\
	X(SPECIAL_RECOGNITION_CHARACTER_ANIMATION,58)\
	X(HIGH_SCORE,59)\
	X(TEACHER_PIONEER,60)\
	X(BEST_CRAFTSMANSHIP,61)\
	X(BEST_DEFENSIVE_MATCH,62)\
	X(PLAY_OF_THE_DAY,63)\
	X(PROGRAMMING,64)\
	X(PROFESSIONALISM,65)\
	X(GOLDEN_CORNDOG,66)

enum class Award_type{
	#define X(A,B) A,
	AWARD_TYPES
	#undef X
};
std::ostream& operator<<(std::ostream&,Award_type);

#define AWARD_ITEMS \
	X(name,std::string)\
	X(award_type,Award_type)\
	X(event_key,std::string)\
	X(recipient_list,Recipient)\
	X(year,Year)

struct Award{
	#define X(A,B) B A;
	AWARD_ITEMS
	#undef X
};
std::ostream& operator<<(std::ostream&,Award const&);

std::vector<Award> team_event_awards(Team,std::string event_key);

struct Video{
	std::string key,type;
};

#define MATCH_ITEMS \
	X(key,std::string)\
	X(comp_level,std::string)\
	X(set_number,int)\
	X(match_number,int)\
	X(alliances,std::vector<Alliance>)\
	X(score_breakdown,std::map<string,int>)\
	X(event_key,std::string)\
	X(videos,vector<Video>)\
	X(time_string,std::string)\
	X(time,int)

struct Match{
	//todo: add contents
};

#endif
