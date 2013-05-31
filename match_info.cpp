#include "match_info.h"
#include<iostream>
#include "str.h"
#include "util.h"
#include "set.h"
#include "map.h"
#include "team.h"

using namespace std;

Maybe<Competition_level> parse_competition_level(string s){
	#define X(name) if(toupper(s)==""#name) return Competition_level::name;
	X(QUALS)
	X(QUARTERS)
	X(SEMIS)
	X(FINALS)
	#undef X
	return Maybe<Competition_level>();
}

ostream& operator<<(ostream& o,Competition_level c){
	switch(c){
		#define X(name) case Competition_level::name: return o<<""#name;
		X(QUALS)
		X(QUARTERS)
		X(SEMIS)
		X(FINALS)
		#undef X
		default: nyi
	}
}

ostream& operator<<(ostream& o,Match_info::Alliance const& a){
	return o<<"Alliance("<<a.score<<" "<<a.teams<<")";
}

ostream& operator<<(ostream& o,Match_info const& m){
	o<<"Match_info("<<m.key<<","<<m.match_number<<",";
	o<<m.teams<<",";
	o<<m.alliances;
	return o<<")";
}

bool operator<(Match_info::Alliance const& a,Match_info::Alliance const& b){
	#define X(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;
	X(score);
	X(teams);
	#undef X
	return 0;
}

set<Competition_level> competition_level(vector<Match_info> const& m){
	set<Competition_level> r;
	for(auto& a:m) r|=a.competition_level;
	return r;
}

bool ok(Match_info const& m){
	set<Team> listed;
	for(auto a:values(m.alliances)){
		listed|=to_set(a.teams);
	}
	auto a=m.teams;
	if(a!=listed){
		//cout<<"Diff: "<<symmetric_difference(a,listed)<<"\n";
		//exit(1);
	}
	return listed==a;
}

template<typename T>
T max(vector<T> const& v){
	T r=*begin(v);
	for(auto a:v) r=max(r,a);
	return r;
}

template<typename T>
T min(vector<T> const& v){
	T r=*begin(v);
	for(auto a:v) r=min(r,a);
	return r;
}

template<typename Func,typename T>
T with_max(Func f,vector<T> const& v){
	return max(mapf([&](T t){ return make_pair(f(t),t); },v)).second;
}

template<typename Func,typename T>
T with_min(Func f,vector<T> const& v){
	return min(mapf([&](T t){ return make_pair(f(t),t); },v)).second;
}

set<Team> teams(Match_info const& m){
	set<Team> r;
	for(auto a:m.alliances){
		//set<string> t1=a.second.teams;
		r|=a.second.teams;
	}
	return r;
}

set<Team> teams(vector<Match_info> const& v){
	set<Team> r;
	for(auto a:v) r|=teams(a);
	return r;
}

vector<Match_info::Alliance> alliances(vector<Match_info> const& m){
	return flatten(mapf([](Match_info m){ return values(m.alliances); },m));
}

vector<int> scores(vector<Match_info> const& m){
	return mapf([](Match_info::Alliance m){ return m.score; },alliances(m));
}

vector<Match_info::Alliance> winning_alliances(vector<Match_info> const& matches){
	return mapf([](Match_info m)->Match_info::Alliance{
		return with_max([](Match_info::Alliance m)->int{ return m.score; },values(m.alliances));
	},matches);
}

vector<Match_info::Alliance> losing_alliances(vector<Match_info> const& m){
	return mapf([](Match_info m)->Match_info::Alliance{
		return with_min([](Match_info::Alliance m)->int{ return m.score; },values(m.alliances));
	},m);
}

double mean_score(vector<Match_info::Alliance> const& v){
	return mean(mapf([](Match_info::Alliance a){ return a.score; },v));
}

