#include "points.h"
#include<vector>
#include<iostream>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "team.h"
#include "match_info.h"
#include "util.h"
#include "scrape.h"
#include "set.h"
#include "calculate.h"
#include "pick.h"
#include "str.h"

using namespace std;

template<typename A,typename B>
vector<B> seconds(vector<pair<A,B>> v){
	return mapf([](pair<A,B> p){ return p.second; },v);
}

vector<Event_key> first_two_events(set<Event_key> e){
	auto s=sorted(mapf([](Event_key e){ return make_pair(get_details(e).start_date,e); },e));
	auto t=take(2,s);
	return seconds(t);
}

vector<Event_key> first_two_events(Team team,Year year){
	//first find events attended.
	auto e=events(with_team(matches(year),team));
	return first_two_events(e);
}

Record qual_record_at_first_two(Team team,Year year){
	auto keys=first_two_events(team,year);
	Record total;
	for(auto event:keys){
		auto m=with_event(matches(year),event);
		m=quals(m);//nyi//need to only look at qualification matches.
		auto c=calculate_records(m);
		total+=c[team];
	}
	return total;
}

unsigned qualifying_matches_won(Team team,Year year){
	return qual_record_at_first_two(team,year).win;
}

int qualifying_ties(Team team,Year year){
	return qual_record_at_first_two(team,year).tie;
}

typedef string Award_name;

int award_points(Award_name a){
	if(a=="Chairman's") return 10;
	if(a=="Engineering Inspiration") return 8;

	//todo see what the 'judged team awards' are
	nyi//if(a==) return 5;
}

int team_age_points(int previous_years){
	assert(previous_years>=0);
	if(previous_years==0) return 10;
	if(previous_years==1) return 5;
	return 0;
}

Year year(Event_key event){
	return atoi(event);
}

Event_picks guess_pick_order(Event_key event){
	//for the 2013 events can actually look at the data from the spreadsheet and would be able to cross check.  Should be able to figure out the captains 100% but the others will be just guesses.
	auto y=year(event);
	if(y==2013){
		string code=toupper(event.c_str()+4);
		static auto p=parse_picks();
		//cout<<p<<"\n";
		auto f=p.find(code);
		assert(f!=p.end());
		return f->second;
	}
	cout<<"GUessing for event:"<<event<<"\n";
	auto m=with_event(matches(y),event);
	cout<<"y="<<y<<"\n";
	nyi
}

class Alliance_number{
	int x;

	public:
	Alliance_number(int a):x(a){
		assert(a>=1 && a<=8);
	}

	operator int()const{ return x; }
};

bool operator==(Alliance_number,Alliance_number)nyi

typedef unsigned Team_index;//0-2

struct Pick_position{
	Alliance_number alliance;
	Team_index pick;
};

bool operator==(Pick_position,Pick_position)nyi

ostream& operator<<(ostream&,Pick_position)nyi

Maybe<Pick_position> pick_position(Event_picks e,Team team){
	for(int a=0;a<8;a++){
		for(Team_index t=0;t<3;t++){
			if(e[a].teams[t]==team){
				return Pick_position{a+1,t};
			}
		}
	}
	return Maybe<Pick_position>();
}

int alliance_captain_points(Alliance_number alliance_number){
	assert(alliance_number>=1 && alliance_number<=8);
	return 17-alliance_number;
}

int alliance_accept_points(int before_this_pick){
	return 17-1-before_this_pick;
}

int picks_before(Pick_position p){
	switch(p.pick){
		case 0: assert(false);
		case 1: return p.alliance-1;
		case 2:nyi
		default: assert(0);
	}
}

int points(Pick_position p){
	if(p.pick==0){
		return alliance_captain_points(p.alliance);
	}
	return alliance_accept_points(picks_before(p));
}

int points(Maybe<Pick_position> m){
	if(m==Maybe<Pick_position>()) return 0;
	return points(*m);
}

int points(Team team,Year year){
	int t=0;
	t+=2*qualifying_matches_won(team,year);
	t+=qualifying_ties(team,year);

	//alliance selection results stuff
	for(auto event:first_two_events(team,year)){
		auto g=guess_pick_order(event);
		//cout<<g<<"\n";
		t+=points(pick_position(g,team));
	}
	
	cout<<"Team "<<team<<" year:"<<year<<" pt"<<t<<"\n";

	//elimination round performance goes here!	
	for(auto event:first_two_events(team,year)){
		auto m=with_event(matches(year),event);
		//m=with_team(m);
		nyi
	}
	nyi

	int previous_years;nyi
	t+=team_age_points(previous_years);
	nyi
}

void points_demo(){
	cout<<points(Team(33),2013)<<"\n";
}

struct Sorting_criteria{
	int points;
	int elimination_round_perf_points;//1
	int highest_elim_round_finish_points;//2
	int alliance_selection_points;//3
	int highest_alliance_selection_points;//4
	int total_qualification_round_points;//5
	int qualification_match_wins;//6
	int highest_individual_match_score;//7
	int second_highest_individual_match_score;//8
	int third_highest_individual_match_score;//9
	//10: random selection
};

ostream& operator<<(ostream& o,Sorting_criteria s){
	o<<"Sorting_criteria(";
	#define X(name) o<<""#name<<":"<<s.name;
	X(points)
	X(elimination_round_perf_points)
	X(highest_elim_round_finish_points)
	X(alliance_selection_points)
	X(highest_alliance_selection_points)
	X(total_qualification_round_points)
	X(qualification_match_wins)
	X(highest_individual_match_score)
	X(second_highest_individual_match_score)
	X(third_highest_individual_match_score)
	#undef X
	return o<<")";
}

bool operator<(Sorting_criteria a,Sorting_criteria b){
    #define X(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;
    X(points)
    X(elimination_round_perf_points)
    X(highest_elim_round_finish_points)
    X(alliance_selection_points)
    X(highest_alliance_selection_points)
    X(total_qualification_round_points)
    X(qualification_match_wins)
    X(highest_individual_match_score)
    X(second_highest_individual_match_score)
    X(third_highest_individual_match_score)
    #undef X
	//at some point would probably be interesting to know which sort criterial were actually used.
	return 0;
}


