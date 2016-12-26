#include<iostream>
#include "team_list.h"
#include "../util.h"
#include "../scrape.h"
#include "../map.h"
#include "../tuple.h"
#include "../str.h"

using namespace std;

template<typename A,typename B>
vector<B> seconds(vector<pair<A,B>> v){
	vector<B> r;
	for(auto p:v) r|=p.second;
	return r;
}

template<typename A,typename B>
pair<A,B>& operator+=(pair<A,B> &a,pair<A,B> b){
	a.first+=b.first;
	a.second+=b.second;
	return a;
}

void print_lines(auto const& a){
	for(auto const& elem:a) cout<<elem<<"\n";
}

tuple<double,int,int> parse_line(string s){
	auto sp=split(s,',');
	if(sp.size()!=3){
		PRINT(s)
	}
	assert(sp.size()==3);
	auto f=atof(sp[0].substr(1,100).c_str());
	auto team1=atoi(sp[1]);
	auto team2=atoi(sp[2]);
	return make_tuple(f,team1,team2);
}

template<typename T>
vector<T> nonempty(vector<T> const& v){
	return filter([](auto const& a){ return !a.empty(); },v);
}

vector<int> current_pnw(){
	auto f=filter(
		[](auto a){
			/*if(a.team_number==1425 || a.team_number==2471){
				PRINT(a);
			}*/
			return a.region=="OR" || a.region=="Oregon" || a.region=="WA" || a.region=="Washington" || a.region=="AK" || a.region=="Alaska";
		},
		all_team_details()
	);
	return mapf([](auto a){ return a.team_number; },f);
}

auto pull_cached(){
	ifstream f("out.txt");
	string line;
	vector<tuple<double,int,int>> info;
	while(f.good()){
		getline(f,line);
		if(line.size()){
			info|=parse_line(line);
		}
	}
	return info;
}

int show_team(int team){
	auto pnw=to_set(current_pnw());

	auto info=pull_cached();
	info=filter([=](auto a){ return pnw.count(get<1>(a)) && pnw.count(get<2>(a)); }, info);
	//info=filter([team](auto a){ return get<2>(a)==team; },info);

	print_lines(info);

	//auto x=lines(slurp("out.txt"));
	//x=nonempty(x);
	//auto m=mapf(parse_line,nonempty(lines(slurp("out.txt"))));
	
	return 0;
}

template<typename A,typename B,typename C>
vector<B> seconds(vector<tuple<A,B,C>> const& in){
	return mapf([](auto a){ return get<1>(a); },in);
}

int top(){
	auto p=pull_cached();
	auto teams=to_set(seconds(p));
	for(auto team:teams){
		auto t=take(10,filter([team](auto a){ return get<2>(a)==team && get<1>(a)!=team; },p));
		cout<<team<<seconds(t)<<"\n";
	}
	return 0;
}

int main(int argc,char **argv){
	auto a=args(argc,argv);
	if(a.size()==2 && a[0]=="--team"){
		return show_team(atoi(a[1]));
	}
	if(a.size()==1 && a[0]=="--top"){
		return top();
	}

	//for every award type that's been given out, calculate a fraction of the time that it's been available where a team has won it
	//this will produce a list of floats for each team
	//next, make a similarity score between each pair of teams based on how different the floats are
	//depending on how slow this is, write the results out to a file, or just allow the user to query a team number

	//could also try to print out all of the awards for a single team and say how far above or below par they are for each award.
	/*
	most interesting awards to have on a radar chart:
	Our highest-rate stuff:
	GP
	Subdivision finalist
	Woodie Flowers
	Innovation in Control
	Excellence in Engineering
	Visualization

	Most above expected:
	Winner
	Finalist
	GP
	Innovation in control
	Excellence in Engineering
	Rookie Inspiration
	*/

	//first, need to get the list of all the teams
	auto t=all_team_details();
	//PRINT(t);
	auto team_numbers=mapf([](auto a){ return a.team_number; },t);
	//auto team_numbers=take(100,mapf([](auto a){ return a.team_number; },t));
	//auto team_numbers=vector<int>{180};
	//PRINT(team_numbers);

	map<int,vector<pair<double,string>>> by_team;

	for(auto team:team_numbers){
		//cout<<team<<"\n";
		//go figure out what events it's been to.
		auto hist=get_team_history(team);
		//PRINT(hist);

		auto event_keys=mapf([](auto a){ return a.key; },hist);
		//PRINT(event_keys);

		using Award_name=string;
		map<Award_name,pair<long unsigned,double>> at_events;//pair of # of times won and expected # of times won

		/*auto m=flatten(mapf(get_awards,event_keys));
		for(auto award:m){
			//see how many teams there were at the event...
			auto f=at_events.find(award.name);
			PRINT(award)
			nyi
		}*/
		for(auto event_key:event_keys){
			auto teams=event_teams(event_key);
			//PRINT(teams);
			auto awards=get_awards(event_key);
			for(auto award:awards){
				auto to_teams=filter([](auto a){ return a.team_number; },award.recipient_list);
				auto expected=(0.0+to_teams.size())/teams.size();
				if(expected>0){
					auto won=filter([=](auto a){ return a.team_number==team; },award.recipient_list).size();
					/*PRINT(award);
					PRINT(expected);
					PRINT(won);*/

					auto award_name=as_string(award.award_type);//simplify_award_name(award.name);
					auto f=at_events.find(award_name);
					if(f==at_events.end()){
						at_events[award_name]=make_pair(0,0);
					}
					at_events[award_name]+=make_pair(won,expected);
				}
			}
		}

		vector<pair<double,Award_name>> rate;
		for(auto p:at_events){
			rate|=make_pair((p.second.first-p.second.second)/event_keys.size(),p.first);
			//rate|=make_pair((p.second.first/p.second.second),p.first);
		}

		/*PRINT(team);
		print_lines(take(5,reversed(sorted(rate))));*/

		//calculate overall level of award-reception
		by_team[team]=rate;
	}

	vector<tuple<double,int,int>> team_scores;
	for(auto team1:team_numbers){
		auto t1_data=by_team[team1];
		for(auto team2:team_numbers){
			auto t2_data=by_team[team2];
			double dist=0;
			for(auto name:to_set(seconds(t1_data))|to_set(seconds(t2_data))){
				auto get=[](auto const& data,auto const& key){
					/*auto f=data.find(key);
					if(f==data.end()) return 0;
					return f->second;*/
					auto f=filter([=](auto x){ return x.second==key; },data);
					if(f.size()==0) return 0.0;
					if(f.size()==1) return f[0].first;
					nyi
				};
				auto lin=get(t1_data,name)-get(t2_data,name);
				dist+=lin*lin;
			}
			team_scores|=make_tuple(dist,team1,team2);
		}
	}

	print_lines(sorted(team_scores));

	return 0;
}
