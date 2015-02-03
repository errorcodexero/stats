#include "scrape.h"

#include<iostream>
#include<typeinfo>
#include<jsoncpp/json/reader.h>
#include<jsoncpp/json/writer.h>
#include "../set.h"

using namespace std;

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

vector<Year> years(){ return range(1992,2016); }

Team::Team(int i):num(i),modifier(0){}

Team::Team(string s){
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

ostream& operator<<(ostream& o,Webcast const& a){
	o<<"Webcast(";
	o<<a.channel<<" "<<a.type<<" file:"<<a.file;
	return o<<")";
}

bool operator<(Webcast const&,Webcast const&){
	nyi
}

bool operator==(Webcast const&,Webcast const&){
	nyi
}

ostream& operator<<(ostream& o,Event const& a){
	o<<"Event(";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	EVENT_ITEMS
	#undef X
	return o<<")";
}

bool operator<(Event const& a,Event const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	EVENT_ITEMS
	#undef X
	return 0;
}

bool operator==(Event const& a,Event const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	EVENT_ITEMS
	#undef X
	return 1;
}

bool operator<=(Event const& a,Event const& b){
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

vector<Event> events(Year year){
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

string key(Event e){ return e.key; }

vector<string> event_keys(vector<Event> v){ return mapf(key,v); }

ostream& operator<<(ostream& o,District a){
	o<<"District(";
	o<<a.key<<" "<<a.name;
	return o<<")";
}

vector<District> districts(Year year){
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

vector<Event> district_events(Year year,string district_key){
	auto g=get_cached("http://www.thebluealliance.com/api/v2/district/"+district_key+"/"+as_string(year)+"/events");
	Json::Value root;
	auto success=Json::Reader().parse(g,root);
	assert(success);
	return convert_elem(root,(vector<Event>*)nullptr);
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

vector<Team_info> teams(){
	vector<Team_info> r;
	for(auto i:range(20)){
		auto g=get_cached("http://thebluealliance.com/api/v2/teams/"+as_string(i));
		Json::Value root;
		auto success=Json::Reader().parse(g,root);
		assert(success);
		r|=convert_elem(root,(vector<Team_info>*)nullptr);
	}
	return r;
}

template<typename T>
T get_data(string url){
	auto g=get_cached(url);
	Json::Value root;
	auto success=Json::Reader().parse(g,root);
	if(!success){
		cout<<"url:"<<url<<"\n";
		cout<<g<<"\n";
	}
	assert(success);
	return convert_elem(root,(T*)nullptr);
}

vector<Event> team_events(Team team,Year year){
	return get_data<vector<Event>>("http://thebluealliance.com/api/v2/team/frc"+as_string(team)+"/"+as_string(year)+"/events");
}

Award convert_elem(Json::Value v,Award*){
	nyi
}

vector<Award> team_event_awards(Team team,string event_key){
	return get_data<vector<Award>>("http://thebluealliance.com/api/v2/team/frc"+as_string(team)+"/event/"+event_key+"/awards");
}
