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

using namespace std;

/*template<typename A,typename B>
ostream& operator<<(ostream& o,pair<A,B> p){
	return o<<"("<<p.first<<","<<p.second<<")";
}*/

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

vector<int> range(int min,int max){
	vector<int> r;
	for(int i=min;i<max;i++) r|=i;
	return r;
}

vector<unsigned> range(unsigned len){
	vector<unsigned> r;
	for(unsigned i=0;i<len;i++) r|=i;
	return r;
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

string escape_to_commandline(char c){
	//NOTE: If you ever want to run this as a cgi script or something, you will neeed to make this better.
	stringstream ss;
	if(c=='&' || c=='-' || c==';') ss<<"\\";
	ss<<c;
	return ss.str();
}

string escape_to_commandline(string const& s){
	stringstream ss;
	for(auto c:s) ss<<escape_to_commandline(c);
	return ss.str();
}

//going to make a cache of downloaded pages...
void download_page(string const& url,string const& outfile){
	string tmpfile="tmp"+as_string(rand())+".tmp";
	stringstream ss;
	//moves the file into place when it's done instead of downloading it to the right place because otherwise if it gets killed halfway through you can end up with an invalid file.
	ss<<"wget ";

	//this line is needed because the blue alliance started requiring that this be sent along with the request.
	ss<<"--header='X-TBA-App-Id: jay:bob:2' ";

	ss<<"-q -O "<<tmpfile<<" "<<escape_to_commandline(url);
	ss<<" && mv "<<tmpfile<<" "<<outfile;
	//cout<<"Running: "<<ss.str()<<"\n";
	auto r=system(ss.str().c_str());
	if(r){
		cout<<"r="<<r<<"\n";
		cout<<"cmd="<<ss.str()<<"\n";
	}
	assert(!r);
}

string escape_url_to_file(string const& url){
        stringstream ss;
        for(char c:url){
                if(c=='/'){
                        ss<<"sl";
                }else{
                        ss<<c;
                }
        }
        return ss.str();
}

void mkdir_p(string const& s){
        //Note: If you care about security, change this to at the very least not let the argument create arbitrary shell commands.
        stringstream ss;
        ss<<"mkdir -p "<<s;
        auto r=system(ss.str().c_str());
        assert(!r);
}

string get_cached(string url){
	static const string CACHE_DIR="this";
	mkdir_p(CACHE_DIR);
	auto file=CACHE_DIR+"/"+escape_url_to_file(url);
	try{
		return slurp(file);
	}catch(...){
		cout<<"Download:"<<url<<"\n";
		download_page(url,file);
		return slurp(file);
	}
}

typedef int Year;

vector<Year> years(){ return range(1992,2016); }

struct Team{
	int num;
	char modifier;

	explicit Team(int i):num(i),modifier(0){}

	explicit Team(string s){
		assert(s.size()>=3);
		assert(s.substr(0,3)=="frc");
		//assert(s.size()<=8);
		s=s.substr(3,10);
		num=atoi(s);
		if(!num){
			//cout<<"was:"<<s<<"\n";
			throw "not valid team #";
		}
		assert(num);
		auto s2=as_string(num);
		auto after=s.substr(s2.size(),8);
		assert(after.size()<2);
		if(after.size()==0){
			modifier=0;
		}else{
			modifier=after[0];
		}
	}
};

ostream& operator<<(ostream& o,Team a){
	o<<a.num;
	if(a.modifier) o<<a.modifier;
	return o;
}

bool operator<(Team a,Team b){
	if(a.num<b.num) return 1;
	if(b.num<a.num) return 0;
	return a.modifier<b.modifier;
}

bool operator==(Team a,Team b){
	return a.num==b.num && a.modifier==b.modifier;
}

struct Alliance{
	vector<Team> declines;
	vector<Team> picks;
};

ostream& operator<<(ostream& o,Alliance const& a){
	o<<"Alliance(";
	o<<"declines:"<<a.declines;
	o<<"picks:"<<a.picks;
	return o<<")";
}

bool operator<(Alliance const& a,Alliance const& b){
	if(a.declines<b.declines) return 1;
	if(b.declines>a.declines) return 0;
	return a.picks<b.picks;
}

bool operator==(Alliance const& a,Alliance const& b){
	return a.declines==b.declines && a.picks==b.picks;
}

struct Webcast{
	string channel,type;
	Maybe<string> file;
};

ostream& operator<<(ostream& o,Webcast const& a){
	o<<"Webcast(";
	o<<a.channel<<" "<<a.type<<" file:"<<a.file;
	return o<<")";
}

bool operator<(Webcast const& a,Webcast const& b){
	nyi
}

bool operator==(Webcast const& a,Webcast const& b){
	nyi
}

#define EVENT_ITEMS \
	X(vector<Alliance>,alliances)\
	X(Maybe<string>,end_date)\
	X(string,event_code)\
	X(Maybe<int>,event_district)\
	X(Maybe<string>,event_district_string)\
	X(int,event_type)\
	X(string,event_type_string)\
	X(Maybe<string>,facebook_eid)\
	X(string,key)\
	X(Maybe<string>,location)\
	X(string,name)\
	X(bool,official)\
	X(Maybe<string>,short_name)\
	X(Maybe<string>,start_date)\
	X(Maybe<string>,venue_address)\
	X(vector<Webcast>,webcast)\
	X(Maybe<string>,website)\
	X(int,year)

struct Event{
	#define X(A,B) A B;
	EVENT_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Event const& a){
	o<<"Event(";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	EVENT_ITEMS
	#undef X
	return o<<")";
}

bool operator<(Event a,Event b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	EVENT_ITEMS
	#undef X
	return 0;
}

bool operator==(Event a,Event b){
	#define X(A,B) if(a.B!=b.B) return 0;
	EVENT_ITEMS
	#undef X
	return 1;
}

bool operator<=(Event a,Event b){
	return a<b || a==b;
}

/*template<typename T>
T convert_elem(Json::Value,T*){
	cout<<"Type:"<<typeid(T).name()<<"\n";
	nyi
}*/

bool convert_elem(Json::Value v,bool*){
	assert(v.isBool());
	return v.asBool();
}

int convert_elem(Json::Value v,int*){
	if(!v.isInt()) throw "not int";
	return v.asInt();
}

string convert_elem(Json::Value v,string*){
	if(!v.isString()) throw "jlksad";
	return v.asString();
}

template<typename T>
vector<T> convert_elem(Json::Value v,vector<T>*){
	assert(v.isArray());
	vector<T> r;
	for(auto elem:v){
		r|=convert_elem(elem,(T*)nullptr);
	}
	return r;
}

Team convert_elem(Json::Value v,Team*){
	if(v.isString()) return Team(v.asString());
	if(v.isInt())nyi
	nyi
}

Alliance convert_elem(Json::Value v,Alliance*){
	assert(v.isObject());
	Alliance r;
	for(auto member:v.getMemberNames()){
		auto value=v[member];
		if(member=="declines"){
			r.declines=convert_elem(value,(vector<Team>*)NULL);
		}else if(member=="picks"){
			//r.picks=convert_elem(value,(vector<int>*)NULL);
			try{
				r.picks=convert_elem(value,(vector<Team>*)NULL);
			}catch(...){
				//this is here due to the 2014 Mexico City picks being garbage
			}
		}else{
			nyi
		}
	}
	return r;
}

Maybe<int> convert_elem(Json::Value v,Maybe<int>*){
	if(v.isInt()) return v.asInt();
	if(v.isNull()) return Maybe<int>();
	throw "wrong type";
}

Maybe<string> convert_elem(Json::Value v,Maybe<string>*){
	if(v.isString()) return v.asString();
	if(v.isNull()) return Maybe<string>();
	throw "wrong type";
}

Webcast convert_elem(Json::Value v,Webcast*){
	assert(v.isObject());
	Webcast r;
	for(auto a:v.getMemberNames()){
		auto value=v[a];
		assert(value.isString());
		auto s=value.asString();
		if(a=="channel"){
			r.channel=s;
		}else if(a=="type"){
			r.type=s;
		}else if(a=="file"){
			r.file=s;
		}else{
			cout<<"a:"<<a<<"\n";
			nyi
		}
	}
	return r;
}

Event convert_elem(Json::Value event,Event*){
	assert(event.isObject());
	Event e;
	for(auto elem:event.getMemberNames()){
		#define X(A,B) if(elem==""#B){ \
			auto value=event[elem];\
			try{\
				e.B=convert_elem(value,(A*)0); \
			}catch(...){\
				cout<<event<<"..."<<""#B<<":"<<value.type()<<value<<"\n"; nyi\
			}\
			continue; \
		}
		EVENT_ITEMS
		#undef X
		auto v=event[elem];
		cout<<elem<<"\n";
		nyi
	}
	return e;
}

vector<Event> events(int year){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/events/"+as_string(year));
	Json::Value root;
	bool success=Json::Reader().parse(g,root);
	assert(success);
	assert(root.isArray());
	vector<Event> r;
	for(auto event:root){
		r|=convert_elem(event,(Event*)0);
		//cout<<"event:"<<event<<"\n";
		/*Event e;
		for(auto elem:event.getMemberNames()){
			#define X(A,B) if(elem==""#B){ \
				auto value=event[elem];\
				try{\
					e.B=convert_elem(value,(A*)0); \
				}catch(...){\
					cout<<event<<"..."<<""#B<<":"<<value.type()<<value<<"\n"; nyi\
				}\
				continue; \
			}
			EVENT_ITEMS
			#undef X
			auto v=event[elem];
			cout<<elem<<"\n";
			nyi
		}
		r|=e;*/
	}
	return r;
}

#define TEAM_INFO_ITEMS\
	X(website,Maybe<string>)\
	X(name,Maybe<string>)\
	X(locality,Maybe<string>)\
	X(rookie_year,Maybe<int>)\
	X(region,Maybe<string>)\
	X(team_number,int)\
	X(location,Maybe<string>)\
	X(key,string)\
	X(country_name,Maybe<string>)\
	X(nickname,Maybe<string>)

struct Team_info{
	#define X(A,B) B A;
	TEAM_INFO_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Team_info const& a){
	o<<"Team_info( ";
	#define X(A,B) o<<""#A<<":"<<a.A<<" ";
	TEAM_INFO_ITEMS
	#undef X
	return o<<")";
}

Team_info convert_elem(Json::Value team_obj,Team_info*){
	assert(team_obj.isObject());
	Team_info t;
	set<string> seen;
	for(auto elem:team_obj.getMemberNames()){
		#define X(A,B) if(elem==""#A){\
			auto value=team_obj[elem];\
			try{\
				t.A=convert_elem(value,(B*)0);\
				seen|=string(""#A);\
			}catch(...){\
				cout<<""#A<<":"<<value<<"\n";nyi\
			}\
		}
		TEAM_INFO_ITEMS
		#undef X
	}

	#define X(A,B) if(seen.count(""#A)==0){\
		cout<<"not found:"#A;\
		nyi\
	}
	TEAM_INFO_ITEMS
	#undef X
	return t;
}

vector<Team_info> teams_at_event(string event_key){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/event/"+event_key+"/teams");
	Json::Value root;
	bool success=Json::Reader().parse(g,root);
	assert(success);
	assert(root.isArray());
	vector<Team_info> r;
	for(auto team_obj:root){
		r|=convert_elem(team_obj,(Team_info*)nullptr);
	}
	return r;
}

template<typename Func,typename T>
void pmap(Func f,vector<T> v){
	set<pid_t> children;
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
	}
	while(children.size()){
		int status=0;
		auto p=wait(&status);
		auto f=children.find(p);
		assert(f!=children.end());
		children.erase(f);
		cout<<"fin "<<p<<" "<<status<<"\n";
	}
}

string key(Event e){ return e.key; }

vector<string> event_keys(vector<Event> v){ return mapf(key,v); }

vector<string> pnw_events(){
	nyi
}

struct District{
	string name,key;
};

ostream& operator<<(ostream& o,District a){
	o<<"District(";
	o<<a.key<<" "<<a.name;
	return o<<")";
}

vector<District> districts(int year){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/districts/"+as_string(year));
	Json::Value root;
	auto success=Json::Reader().parse(g,root);
	assert(success);
	return convert_elem(root,(vector<District>*)0);
}

District convert_elem(Json::Value a,District*){
	assert(a.isObject());
	District r;
	for(auto elem:a.getMemberNames()){
		auto value=a[elem];
		auto s=convert_elem(value,(string*)0);
		if(elem=="name"){
			r.name=s;
		}else if(elem=="key"){
			r.key=s;
		}else{
			nyi
		}
	}
	return r;
}

vector<Event> district_events(int year,string district_key){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/district/"+district_key+"/"+as_string(year)+"/events");
	Json::Value root;
	auto success=Json::Reader().parse(g,root);
	assert(success);
	return convert_elem(root,(vector<Event>*)nullptr);
}

vector<Team> team_numbers(vector<Team_info> v){
	return mapf([](Team_info t){ return Team{t.team_number}; },v);
}

Team_info team_info(Team team){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/team/frc"+as_string(team));//may want to do something with the team #'s that have modifiers
	Json::Value root;
	auto success=Json::Reader().parse(g,root);
	if(!success){
		cout<<"Fail:team "<<team<<"\n";
	}
	assert(success);
	return convert_elem(root,(Team_info*)nullptr);
}

void district_graph(string district_name){
	map<string,set<Team>> by_event;
	auto events=district_events(2015,district_name);
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
		if(d=="2015-02-26" || d=="2015-02-27") return 1;
		if(d=="2015-03-06" || d=="2015-03-05") return 2;
		if(d=="2015-03-12" || d=="2015-03-13") return 3;
		if(d=="2015-03-20" || d=="2015-03-19") return 4;
		if(d=="2015-03-26" || d=="2015-03-27") return 5;
		if(d=="2015-04-02") return 6;
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
			if(x){
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

int main(){
	district_graph("in");
	//mapf([](int i){ return team_info(Team{i}); },range(1,6000));
}
