#include "scrape.h"
#include<iostream>
#include<string.h>
#include "util.h"
#include "calculate.h"
#include "record.h"
#include "tuple.h"
#include "set.h"

using namespace std;

/*
This is designed to project the strength of different events by giving the winning % of the teams appearing at that event this year in the previous year
*/

bool nw_event(Event_v2 b){
	return !!strstr(b.name.c_str(),"PNW");
}

double win_pcnt_last_year(Team team){
	//cout<<"Team:"<<team<<"\n";
	static const vector<Match_info> m=matches(2013);
	return win_portion(calculate_records(m)[team]);
}

double get24thbest(vector<double> v){
	v=reversed(sorted(v));
	if(v.size()<24) return 0;//assert(v.size()>=24);
	return v[23];
}

int project(map<string,vector<string>> /*flags*/){
	//cout<<get_event_keys(2014)<<"\n";
	auto events=get_events(2014);
	auto nw_events=filter(nw_event,events);

	auto matches2013=matches(2013);
	for(unsigned i=1990;i<2013;i++){
		matches2013|=matches(i);
	}
	auto records2013=calculate_records(matches2013);
	auto old_win_pct=[&](Team team){ return win_portion(records2013[team]); };

	vector<tuple<double,double,string>> data;
	for(auto a:nw_events){
		//cout<<a.name_short<<"\t";
		auto t=get_details(a.key).teams;
		auto m=mapf(old_win_pct,t);
		//cout<<sorted(m)<<"\n";
		data|=make_tuple(get24thbest(m),mean(m),*a.short_name);
	}
	cout<<data<<"\n";
	for(auto a:sorted(data)){
		cout<<a<<"\n";
	}
	//auto c=calculate_records(matches(2013));
	auto m=mapf(old_win_pct,teams(matches2013));
	cout<<"Worldwide average winning pcnt:"<<mean(m)<<"\n";
	set<Team> nw_teams=[=](){
		auto e=mapf([](auto b){ return b.key; },get_events(2013));
		//cout<<e<<"\n";
		vector<string> nw2013=filter([](string s){ return !!strstr(s.c_str(),"2013wa"); },e);
		nw2013|=string("2013orpo");
		//cout<<nw2013<<"\n";
		auto nw_matches=filter(
			[=](Match_info m){ return contains(nw2013,m.event); },
			matches2013
		);
		return teams(nw_matches);
	}();
	cout<<"nw average:"<<mean(mapf(old_win_pct,nw_teams))<<"\n";
	//mapf(old_win_pct,teams(matches2013
	/*todo: 
	-Figure out what the average winning % is for a team in the PNW area
	-And winning % for a team nationally
	*/
	return 0;
}
