#ifndef SCRAPE_H
#define SCRAPE_H

#include "match_info.h"
#include "bevent.h"
#include "team.h"
#include "nlohmann/json.hpp"

using json=nlohmann::json;

template<typename T>
std::set<T> operator-(std::set<T> a,std::set<T> b){
	for(auto elem:b){
		a.erase(elem);
	}
	return a;
}

std::string scrape_cached(std::string const&);
std::vector<std::string> get_event_keys(int);
std::vector<Match_info> matches(int);

using BDate=std::string;
using Event_district=int;//actually enumerated.

struct Webcast{
	//channel,str
	//type,str
};

struct BAlliance{
	//declines array
	//picks array of teams
};

#define EVENT_V2_ITEMS(X) \
	X(std::string,key)\
	X(Maybe<std::string>,website)\
	X(Maybe<bool>,official)\
	X(Maybe<BDate>,end_date)\
	X(std::string,name)\
	X(Maybe<std::string>,short_name)\
	X(Maybe<std::string>,facebook_eid)\
	X(Maybe<std::string>,venue_address)\
	X(Event_district,event_district)\
	X(std::string,location)\
	X(std::string,event_code)\
	X(int,year)\
	X(std::vector<Webcast>,webcast)\
	X(std::vector<BAlliance>,alliances)\
	X(std::string,event_type_string)\
	X(Maybe<BDate>,start_date)\
	X(int,event_type)\
	X(Maybe<std::string>,event_district_string)\
	X(Maybe<std::string>,timezone)\
	X(Maybe<int>,week)

struct Event_v2{
	#define X(A,B) A B;
	EVENT_V2_ITEMS(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Event_v2 const&);

std::vector<Event_v2> get_events(int year);
std::vector<Event_v2> get_team_history(int team);


//data from the event details page of the blue alliance
struct BEvent_details{
	Maybe<std::string> location;
	std::string key;
	std::string year;//could actually make this an int
	Maybe<Date> start_date;
	std::string name;
	std::vector<Team> teams;
	bool official;
	std::vector<std::string> matches;
	std::string event_code;
	Maybe<Date> end_date;
	Maybe<std::string> name_short;
	Maybe<bool> facebook_eid; //fix the type of this.
};
std::ostream& operator<<(std::ostream&,BEvent_details const&);

BEvent_details get_details(std::string const& event_code);
void awards();
std::vector<int> all_years();
std::vector<Match_info> all_matches();
std::set<Team> all_teams();


//using Json_obj_it=nlohmann::basic_json<>::iter_impl<nlohmann::basic_json<>>;
using Json_obj_it=nlohmann::basic_json<>::iter_impl<const nlohmann::basic_json<>>;

int pull_element(Json_obj_it it,std::string const&,int*);
std::string pull_element(Json_obj_it it,std::string const& name,std::string*);

bool pull_element(Json_obj_it it,std::string const&,bool*);

Date pull_element(Json_obj_it,std::string const&,Date*);

template<typename T>
Maybe<T> pull_element(Json_obj_it it,std::string const& name,Maybe<T>*){
	auto v=it.value();
	if(v.is_null()){
		return Maybe<T>();
	}
	return pull_element(it,name,(T*)0);
}

int as_type(json j,int*);
std::string as_type(json j,std::string*);
Webcast as_type(json j,Webcast*);
BAlliance as_type(json j,BAlliance*);

template<typename T>
std::vector<T> pull_element(Json_obj_it it,std::string key,std::vector<T>*){
	auto v=it.value();
	assert(v.is_array());
	std::vector<T> r;
	for(auto elem:v){
		try{
			r|=as_type(elem,(T*)0);
		}catch(...){
			std::cout<<"Getting "<<key<<"\n";
			throw;
		}
		//PRINT(elem);
		//nyi
	}
	return r;
}

#define RECIPIENT_ITEMS(X)\
	X(Maybe<std::string>,awardee)\
	X(Maybe<int>,team_number)
struct Recipient{
	#define X(A,B) A B;
	RECIPIENT_ITEMS(X)
	#undef X
};

enum class Award_type{
    CHAIRMANS = 0,
    WINNER = 1,
    FINALIST = 2,

    WOODIE_FLOWERS = 3,
    DEANS_LIST = 4,
    VOLUNTEER = 5,
    FOUNDERS = 6,
    BART_KAMEN_MEMORIAL = 7,
    MAKE_IT_LOUD = 8,

    ENGINEERING_INSPIRATION = 9,
    ROOKIE_ALL_STAR = 10,
    GRACIOUS_PROFESSIONALISM = 11,
    COOPERTITION = 12,
    JUDGES = 13,
    HIGHEST_ROOKIE_SEED = 14,
    ROOKIE_INSPIRATION = 15,
    INDUSTRIAL_DESIGN = 16,
    QUALITY = 17,
    SAFETY = 18,
    SPORTSMANSHIP = 19,
    CREATIVITY = 20,
    ENGINEERING_EXCELLENCE = 21,
    ENTREPRENEURSHIP = 22,
    EXCELLENCE_IN_DESIGN = 23,
    EXCELLENCE_IN_DESIGN_CAD = 24,
    EXCELLENCE_IN_DESIGN_ANIMATION = 25,
    DRIVING_TOMORROWS_TECHNOLOGY = 26,
    IMAGERY = 27,
    MEDIA_AND_TECHNOLOGY = 28,
    INNOVATION_IN_CONTROL = 29,
    SPIRIT = 30,
    WEBSITE = 31,
    VISUALIZATION = 32,
    AUTODESK_INVENTOR = 33,
    FUTURE_INNOVATOR = 34,
    RECOGNITION_OF_EXTRAORDINARY_SERVICE = 35,
    OUTSTANDING_CART = 36,
    WSU_AIM_HIGHER = 37,
    LEADERSHIP_IN_CONTROL = 38,
    NUM_1_SEED = 39,
    INCREDIBLE_PLAY = 40,
    PEOPLES_CHOICE_ANIMATION = 41,
    VISUALIZATION_RISING_STAR = 42,
    BEST_OFFENSIVE_ROUND = 43,
    BEST_PLAY_OF_THE_DAY = 44,
    FEATHERWEIGHT_IN_THE_FINALS = 45,
    MOST_PHOTOGENIC = 46,
    OUTSTANDING_DEFENSE = 47,
    POWER_TO_SIMPLIFY = 48,
    AGAINST_ALL_ODDS = 49,
    RISING_STAR = 50,
    CHAIRMANS_HONORABLE_MENTION = 51,
    CONTENT_COMMUNICATION_HONORABLE_MENTION = 52,
    TECHNICAL_EXECUTION_HONORABLE_MENTION = 53,
    REALIZATION = 54,
    REALIZATION_HONORABLE_MENTION = 55,
    DESIGN_YOUR_FUTURE = 56,
    DESIGN_YOUR_FUTURE_HONORABLE_MENTION = 57,
    SPECIAL_RECOGNITION_CHARACTER_ANIMATION = 58,
    HIGH_SCORE = 59,
    TEACHER_PIONEER = 60,
    BEST_CRAFTSMANSHIP = 61,
    BEST_DEFENSIVE_MATCH = 62,
    PLAY_OF_THE_DAY = 63,
    PROGRAMMING = 64,
    PROFESSIONALISM = 65,
    GOLDEN_CORNDOG = 66,
    MOST_IMPROVED_TEAM = 67,
	WILDCARD = 68
};

std::ostream& operator<<(std::ostream&,Award_type);

#define AWARD_ITEMS(X)\
	X(Award_type,award_type)\
	X(std::string,event_key)\
	X(std::string,name)\
	X(std::vector<Recipient>,recipient_list)\
	X(int,year)

struct Award{
	#define X(A,B) A B;
	AWARD_ITEMS(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Award const&);

std::vector<Award> get_awards(std::string event_key);
std::string simplify_award_name(std::string);

#endif
