#include "scrape_stats.h"
#include "json_spirit.h"
#include "util.h"
#include "scrape.h"
#include "tuple.h"
#include "points.h"
#include "set.h"
#include "main.h"

using namespace std;

vector<string> all_event_keys(){
	return flatten(mapf(get_event_keys,all_years()));
}

struct Stats{
	map<Team,double> opr,ccwm,dpr;
};

ostream& operator<<(ostream&,Stats)nyi

typedef string Event_key;

Maybe<Stats> pull_stats(Event_key k){
	auto s=scrape_cached("http://www.thebluealliance.com/api/v2/event/"+k+"/stats");
	json_spirit::Value value;
	read(s,value);
	if(value.type()==6) return Maybe<Stats>();

	Stats r;
	//cout<<value.type()<<endl;
	for(auto a:value.get_obj()){
		auto n=a.name_;
		auto v=a.value_;
		if(n=="oprs" || n=="dprs" || n=="ccwms"){
			map<Team,double> m;
			for(auto b:v.get_obj()){
				//write(b,cout,json_spirit::pretty_print);
				auto n1=b.name_;
				auto v1=b.value_;
				//cout<<"n1:"<<n1<<endl;
				m[Team(n1)]=v1.get_real();
			}
			if(n=="oprs") r.opr=m;
			else if(n=="dprs") r.dpr=m;
			else if(n=="ccwms") r.ccwm=m;
			else nyi
		}else{
			nyi
		}
	}
	return r;
}

template<typename T>
vector<T> non_null(vector<Maybe<T>> v){
	vector<T> r;
	for(auto a:v){
		if(a) r|=*a;
	}
	return r;
}

typedef tuple<Event_key,Team,double> Flat_stat;
vector<tuple<Event_key,Team,double>> flatten(vector<pair<Event_key,Stats>> v){
	vector<tuple<Event_key,Team,double>> r;
	for(auto a:v){
		for(auto p:a.second.ccwm){
			r|=make_tuple(a.first,p.first,p.second);
		}
	}
	return r;
}

template<typename Func,typename K,typename V>
map<K,V> filter_map(Func f,map<K,V> m){
	//map<K,V> r;
	nyi
	//return r;
}

vector<Flat_stat> grab(Event_key k){
	vector<Event_key> v;
	v|=k;
	auto m=mapf([](string s){ return make_pair(s,pull_stats(s)); },v);
	//auto m2=non_null(m);//filter([](Maybe<Stats> m){ return !!m; },m);
	auto m2=filter(
		[](pair<string,Maybe<Stats>> p){ return p.second; },
		m
	);
	auto m3=mapf(
		[](pair<string,Maybe<Stats>> p){ return make_pair(p.first,*p.second); },
		m2
	);
	//cout<<m.size()<<" "<<m2.size()<<endl;
	//auto with_data=map
	auto m4=flatten(m3);
	//for(auto a:m3) cout<<a<<endl;
	auto s=sort_tuples<Flat_stat,2>(m4);//reverse_pairs(m3));
	return s;
}

void print_extremes(Event_key k){
	auto s=grab(k);
	//assert(s.size());
	if(s.size()==0) return;
	cout<<s[0]<<endl;
	cout<<s[s.size()-1]<<endl;
}

vector<Flat_stat> grab_all(){
	//auto events=filter(
	//	[](string s){
	//		return !contains({/*"2007sc","2007sac","2007tx","2007ny","2007il"*/},s);
			//return s!="2007sc" && s!="2007sac" && s!="2007tx";
	//	},
	//	all_event_keys()
	//);
	return flatten(mapf(grab,all_event_keys()));
}

/*template<typename T>
vector<T> take(unsigned lim,vector<T> v){
	vector<T> r;
	for(unsigned i=0;i<lim && i<v.size();i++){
		r|=v[i];
	}
	return r;
}*/

template<typename T>
vector<T> head(vector<T> v){
	return take(10,v);
}

set<Event_key> events_by_year(Year year){
	return to_set(mapf([](BEvent b){ return b.key; },get_events(year)));
	nyi //return to_set(mapf([](BEvent_details b){ return b.key; },get_events(year)));
}

vector<double> oprs_by_year(Year year){
	vector<double> r;
	for(auto event:events_by_year(year)){
		for(auto elem:grab(event)){
			r|=get<2>(elem);
		}
	}
	return r;
}

vector<int> to_ints(vector<double> v){
	return mapf([](double d){ return (int)d; },v);
}

void scrape_stats_demo(){
	auto a=all_event_keys();
	//cout<<a<<endl;
	/*for(auto elem:a){
		print_extremes(elem);
	}*/
	/*for(auto a:enumerate(grab("2007sc"))){
		cout<<a<<endl;
	}*/
	{
		auto a=grab_all();
		a=sort_tuples<Flat_stat,2>(a);
		cout<<head(a)<<endl;
		cout<<a[0]<<endl;
		cout<<a[a.size()-1]<<endl;
		cout<<a.size()<<endl;
		//cout<<a<<endl;
	}
	//todo: show quartiles of OPR by year
	//then, show that normalized to match scores
	for(auto year:all_years()){
		//auto e=events_by_year(year);
		cout<<year<<" ";
		/*cout<<e<<endl;
		for(auto event:e){
			auto g=grab(event);
			cout<<event<<":"<<g<<endl;
		}*/
		auto a=oprs_by_year(year);
		if(a.size()){
			//double m=*median(a);
			double m=median_score(year);
			//double m=average_score(year);
			//cout<<"th"<<m<<endl;
			a/=(m/100);
			for(auto e:distrib(to_ints(a))){
				cout<<e<<" ";
			}
			cout<<endl;
		}else{
			cout<<"NULL"<<endl;
		}
	}
}
