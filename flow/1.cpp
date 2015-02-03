#include<iostream>
#include<sstream>
#include<utility>
#include<typeinfo>
#include<sys/wait.h>
#include<unistd.h>
#include<jsoncpp/json/reader.h>
#include<jsoncpp/json/writer.h>
#include "../util.h"
#include "../set.h"
#include "scrape.h"

using namespace std;

template<typename A,typename B>
vector<A> firsts(vector<pair<A,B>> v){
	vector<A> r;
	for(auto a:v) r|=a.first;
	return r;
}

template<typename A,typename B>
vector<B> seconds(vector<pair<A,B>> v){
	vector<B> r;
	for(auto a:v) r|=a.second;
	return r;
}

template<typename T>
vector<T> operator|(vector<T> a,vector<T> b){
	a|=b;
	return a;
}

template<typename T>
set<T> operator&(set<T> a,set<T> b){
	set<T> r;
	for(auto elem:a){
		if(b.count(elem)) r|=elem;
	}
	return r;
}

template<typename T>
multiset<T>& operator|=(multiset<T>& m,T t){
	m.insert(t);
	return m;
}

template<typename T>
multiset<T> to_multiset(vector<T> v){
	multiset<T> r;
	for(auto a:v) r|=a;
	return r;
}

template<typename T>
set<T> to_set(multiset<T> m){
	set<T> r;
	for(auto a:m) r|=a;
	return r;
}

template<typename T>
ostream& operator<<(ostream& o,multiset<T> const& a){
	o<<"{ ";
	for(auto elem:to_set(a)){
		o<<a.count(elem)<<":"<<elem<<" ";
	}
	return o<<"}";
}

template<typename K,typename V>
vector<V> values(map<K,V> m){
	vector<V> r;
	for(auto a:m) r|=a.second;
	return r;
}

template<typename T>
set<T> or_all(vector<set<T>> v){
	set<T> r;
	for(auto a:v) r|=a;
	return r;
}

template<typename Func,typename T>
void pmap(Func f,vector<T> v){
	set<pid_t> children;

	auto wait1=[&](){
		int status=0;
		auto p=wait(&status);
		auto f=children.find(p);
		assert(f!=children.end());
		children.erase(f);
		cout<<"fin "<<p<<" "<<status<<"\n";
	};

	for(auto a:v){
		rand();
		int pid=fork();
		assert(pid>=0);
		if(pid==0){
			//child
			f(a);
			exit(0);
		}
		children|=pid;
		while(children.size()>100){
			wait1();
		}
	}
	while(children.size()){
		wait1();
	}
}

vector<Team> team_numbers(vector<Team_info> v){
	return mapf([](Team_info t){ return Team{t.team_number}; },v);
}

void district_graph(string district_name){
	map<string,set<Team>> by_event;
	//auto events=district_events(2015,district_name);
	auto events=::events(2015);
	for(auto event:events){
		auto key=event.key;
		//cout<<key<<"\n";
		auto t=teams_at_event(key);
		auto nums=team_numbers(t);
		by_event[key]=to_set(nums);
	}

	//cout<<by_event<<"\n";
	set<Team> teams=or_all(values(by_event));
	//cout<<teams<<"\n";

	//for each team, go throught all of its events and add each of the edges
	sort(begin(events),end(events),[](Event a,Event b){ return a.start_date<b.start_date; });

	vector<pair<string,string>> trips;
	for(auto team:teams){
		vector<string> path;//{"start"};
		for(auto event:events){
			if(by_event[event.key]&team){
				path|=event.key;
			}
		}
		//path|=string("end");
		for(unsigned i=0;i+1<path.size();i++){
			trips|=make_pair(path[i],path[i+1]);
		}
	}
	//cout<<t;
	auto trip_counts=to_multiset(trips);
	auto all=to_set(firsts(trips)|seconds(trips));

	auto m=to_multiset(mapf([](Team t){ return team_info(t).region; },teams));
	//cout<<m<<"\n";

	auto find_date=[&](string event_key)->string{
		for(auto event:events){
			if(event.key==event_key){
				assert(event.start_date);
				return *event.start_date;
			}
		}
		if(event_key=="start") return "2015";
		if(event_key=="end") return "2016";
		assert(0);
	};

	auto week_no=[&](string event_key)->int{
		auto d=find_date(event_key);
		if(d=="2015-02-26" || d=="2015-02-27" || d=="2015-02-25") return 1;
		if(d=="2015-03-06" || d=="2015-03-05" || d=="2015-03-04" || d=="2015-03-09") return 2;
		if(d=="2015-03-12" || d=="2015-03-13" || d=="2015-03-11") return 3;
		if(d=="2015-03-20" || d=="2015-03-19" || d=="2015-03-18") return 4;
		if(d=="2015-03-26" || d=="2015-03-27" || d=="2015-03-25") return 5;
		if(d=="2015-04-01" || d=="2015-04-02") return 6;
		cout<<d<<"\n";
		assert(0);
	};

	auto by_week=segregate(week_no,to_vector(all));

	cout<<"digraph G{\n";

	auto show=[](string s){ return s.substr(4,20); };

	/*int i=0;
	for(auto week:by_week){
		cout<<"\tsubgraph cluster_"<<i<<"{\n";
		i++;
		for(auto event:week.second){
			cout<<"\t\t\""<<show(event)<<"\";\n";
		}
		cout<<"\t\tcolor=red;\n";
		cout<<"\t\tlabel=\""<<week.first<<"\"\n";
		//cout<<"\t\trank=min;\n";
		cout<<"\t}\n";
	}

	for(int j=0;j<i;j++){
		cout<<"\t{rank=min cluster_"<<i<<" cluster_"<<j<<"}\n";
	}*/

	for(auto week:by_week){
		cout<<"\t{rank=same ";
		for(auto elem:week.second){
			cout<<show(elem)<<" ";
		}
		cout<<"}\n";
	}

	for(auto event1:all){
		for(auto event2:all){
			if(event2<=event1) continue;
			bool swap1=find_date(event1)<find_date(event2);
			auto k1=event1,k2=event2;
			if(!swap1) swap(k1,k2);
			
			auto x=trip_counts.count(make_pair(k1,k2));
			if(x>4){
				//cout<<"\t\""<<k1<<"\"->\""<<k2<<"\" [label=\""<<x<<"\" penwidth="<<x<<"];\n";
				string style="solid";
				if(x<8) style="dashed";
				if(x<3) style="dotted";
				cout<<"\t\""<<k1.substr(4,20)<<"\"->\""<<k2.substr(4,20)<<"\" [label=\""<<x<<"\" weight="<<x<<" style="<<style<<"];\n";
			}
		}
	}
	cout<<"}";
	/*cout<<"digraph G{\n";
	for(auto event1:sorted(events)){
		//cout<<"\t\""<<event1.key<<"\" [label=\""<<event1.start_date<<"\"];\n";
		for(auto event2:events){
			if(event2<=event1) continue;
			auto common=by_event[event1.key]&by_event[event2.key];
			auto k1=event1.key,k2=event2.key;
			bool swap1=event1.start_date<event2.start_date;
			if(!swap1) swap(k1,k2);
			
			auto x=trip_counts.count(make_pair(k1,k2));
			if(x){
				cout<<"\t\""<<k1<<"\"->\""<<k2<<"\" [label=\""<<x<<"\" penwidth="<<x<<"];\n";
			}
		}
	}
	cout<<"}";*/

	/*for(auto t:teams){
		team_info(
	}*/

	/*for(auto year:years()){
		auto d=districts(year);
		for(auto dist:d){
			auto e=district_events(year,dist.key);
			cout<<e<<"\n";
		}
	}
	//download_page("http://www.thebluealliance.com/api/v2/events/1992","1.tmp");
	//cout<<slurp("1.tmp")<<"\n";
	for(auto year:years()){
		auto e=events(year);
		//pmap(teams_at_event,event_keys(e));
		for(auto event:e){
			cout<<event.key<<"\n";
			try{
				auto t=teams_at_event(event.key);
				//cout<<t<<"\n";
			}catch(const char *s){
				cout<<"caught:"<<s<<"\n";
				nyi
			}
		}
	}*/

}

template<typename A,typename B>
vector<pair<A,B>> cross(vector<A> a,vector<B> b){
	vector<pair<A,B>> r;
	for(auto a1:a){
		for(auto b1:b){
			r|=make_pair(a1,b1);
		}
	}
	return r;
}

int main(){
	district_graph("in");
	//mapf([](int i){ return team_info(Team{i}); },range(1,6000));
	#if 0
	auto t=teams();
	cout<<"total:"<<t.size()<<"\n";
	auto teams=mapf([](Team_info t){ return Team{t.key}; },t);
	/*for(auto a:t){
		team_info(Team{a.key});
	}*/
	for(auto year:years()){
		for(auto a:teams){
			team_events(a,year);
		}
	}
	//pmap([](pair<Year,Team> p){ team_events(p.second,p.first); },cross(years(),teams));
	cout<<"done\n";
	#endif
}
