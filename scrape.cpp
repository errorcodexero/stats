#include "scrape.h"
#include<sstream>
#include<cassert>
#include<string.h>
#include<sys/time.h>
#include<boost/numeric/ublas/matrix.hpp>
#include<boost/numeric/ublas/vector.hpp>
#include<boost/numeric/ublas/lu.hpp>
#include<boost/numeric/ublas/io.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
//#include "json_spirit.h"
#include "nlohmann/json.hpp"
#include "util.h"
#include "str.h"
#include "map.h"
#include "set.h"
#include "team.h"
#include "bevent.h"
#include "match_info.h"

using namespace nlohmann;
using namespace std;

//using Json_obj_it=nlohmann::basic_json<>::iter_impl<nlohmann::basic_json<>>;

template<typename K,typename V>
vector<pair<K,V>> map_to_pairs(map<K,V> m){
	vector<pair<K,V>> r;
	for(auto p:m) r|=make_pair(p.first,p.second);
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

string scrape_webpage(string const& url){
	string tmpfile="tmp1.tmp"; //todo: make this use the built-in tmpfile name generation.
	download_page(url,tmpfile);
	return slurp(tmpfile);
	//probably want to delete the temprorary file...
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

string scrape_cached(string const& url){
	static int made_dir=0;
	if(!made_dir){
		mkdir_p("this");
		made_dir=1;
	}

	auto cache_file=string("this/")+escape_url_to_file(url);
	try{
		return slurp(cache_file);
	}catch(...){}
	cout<<"not found, redownloading:"<<url<<"\n";
	cout.flush();
	download_page(url,cache_file);
	auto s=slurp(cache_file);
	if(s.size()){
		//cout<<"Size was:"<<s.size()<<"\n";
		return s;
	}
	cout<<"Trying again...\n";
	download_page(url,cache_file);
	auto s2=slurp(cache_file);
	return s2;
}

vector<string> split_on(string const& haystack,string const& needle){
	vector<string> r;
	stringstream ss;
	for(unsigned i=0;i<haystack.size();i++){
		if(haystack.substr(i,needle.size())!=needle){
			ss<<haystack[i];
		}else{
			r.push_back(ss.str());
			ss.str("");
			i+=needle.size()-1;
		}
	}
	r.push_back(ss.str());
	return r;
}

string quoted(string const& s){
	stringstream ss;
	ss<<"\""<<s<<"\"";
	return ss.str();
}

string inside_td(string const& s){
	return split_on(split_on(s,"<td>")[1],"</td>")[0];
}

//this is incomplete.
string interpret_html_escapes(string const& s){
	stringstream ss;
	for(unsigned i=0;i<s.size();i++){
		const string m="&nbsp;";
		if(s.substr(i,m.size())==m){
			ss<<" ";
			i+=m.size()-1;
		}else{
			ss<<s[i];
		}
	}
	return ss.str();
}

typedef string Regional_type;
typedef string Match;

struct Event{
	string name;
	Regional_type type;
	string venue,city,dates,url;
};

ostream& operator<<(ostream& o,Event const& e){
	return o<<"Event("<<e.name<<")";
}

vector<Event> events(){
	auto url="https://my.usfirst.org/myarea/index.lasso?event_type=FRC&year=2013";
	auto page=scrape_cached(url);
	//cout<<page.size()<<"\n";
	auto pts=split_on(page,"<tr bgcolor=\"#FFFFFF\">");
	//cout<<pts.size()<<" "<<skip(2,pts).size()<<"\n";
	vector<Event> r;
	//we may be skipping the championship event
	for(auto part:skip(2,pts)){
		//cout<<"yx ";
		auto p=split_on(part,"</tr>");
		auto d=p.at(0);
		auto l=lines(d);
		assert(l.size()>=10);
		//cout<<l<<"\n";
		auto regional_type=inside_td(l[2]);
		auto event_name=rstrip(split(split(l[5],'>')[1],'<')[0]);
		string event_url;
		{
			auto a=split_on(split_on(l[5],"href=\"")[1],"\">")[0];
			event_url=split(url,'?')[0]+a;
		}

		auto venue=inside_td(l[7]);
		auto city=interpret_html_escapes(inside_td(l[8]));
		auto dates=inside_td(l[9]);
		//cout<<event_url<<"\n";
		//cout<<regional_type<<" "<<quoted(event_name)<<" ";
		//cout<<city<<"\n";
		//cout<<quoted(venue)<<" "<<dates<<"\n";
		Event e{event_name,regional_type,venue,city,dates,event_url};
		r|=e;
	}
	return r;
}

/*ostream& operator<<(ostream& o,json_spirit::Value const&){
	nyi//write(v,o,json_spirit::pretty_print);
	return o;
}*/

/*ostream& operator<<(ostream& o,json_spirit::Object const&){
	nyi//write(obj,o,json_spirit::pretty_print);
	return o;
}*/

/*template<typename A>
ostream& operator<<(ostream& o,json_spirit::Pair_impl<A> const& p){
	//write(p,o,json_spirit::pretty_print);
	return o<<"("<<p.name_<<" "<<p.value_<<")";
}*/

Maybe<Date> parse_date(Maybe<string> m){
	if(m==Maybe<string>()) return Maybe<Date>();
	auto s=*m;
	auto sp=split(s,'T');
	assert(sp.size()==2);
	auto s2=split(sp[0],'-');
	int year=atoi(s2[0]);
	int month=atoi(s2[1]);
	int day=atoi(s2[2]);
	//ignoring the time for now.  Mostly seem to be zeros anyway.
	return Date(year,month,day);
}

int pull_element(Json_obj_it it,string const& name,int*){
	auto v=it.value();

	if(v.is_number()) return v;

	if(v.is_string()){
		string s=v;
		int i=atoi(s.c_str());
		if(as_string(i)==s){
			return i;
		}
		if(name=="team_number" && i){
			//this will make it ignore suffixes like "971B".
			return i;
		}
	}

	PRINT(name);
	PRINT(v);
	nyi
}

string pull_element(Json_obj_it it,string const& name,string*){
	auto v=it.value();
	if(!v.is_string()){
		PRINT(name);
		PRINT(v);
	}
	assert(v.is_string());
	return v;
}

bool pull_element(Json_obj_it it,string const& name,bool*){
	auto v=it.value();
	if(v.is_boolean()) return v;
	PRINT(name);
	PRINT(it.key());
	PRINT(it.value());
	nyi
}

Date pull_element(Json_obj_it,string,Date*){
	nyi
}

/*template<typename T>
Maybe<T> pull_element(Json_obj_it it,string name,Maybe<T>*){
	auto v=it.value();
	if(v.is_null()){
		return Maybe<T>();
	}
	return pull_element(it,name,(T*)0);
}*/

Recipient as_type(json j,const Recipient*){
	Recipient r;
	for(auto at=j.begin();at!=j.end();++at){
		#define X(A,B) if(at.key()==""#B){\
			r.B=pull_element(at,""#B,(A*)0);\
			continue;\
		}
		RECIPIENT_ITEMS(X)
		#undef X
		PRINT(at.key());
		nyi
	}
	return r;
}

int as_type(json j,int*){
	if(!j.is_number()){
		PRINT(j);
		throw "not int";
	}
	assert(j.is_number());
	return j;
}

string as_type(json j,string*){
	if(j.is_string()){
		nyi
	}
	PRINT(j);
	nyi
}

Webcast as_type(json j,Webcast*){
	return Webcast{};
}

BAlliance as_type(json j,BAlliance*){
	return BAlliance{};
}

/*template<typename T>
vector<T> pull_element(Json_obj_it it,string key,vector<T>*){
	auto v=it.value();
	assert(v.is_array());
	vector<T> r;
	for(auto elem:v){
		try{
			r|=as_type(elem,(T*)0);
		}catch(...){
			cout<<"Getting "<<key<<"\n";
			nyi//throw;
		}
		//PRINT(elem);
		//nyi
	}
	return r;

	PRINT(v.is_null());
	PRINT(v.is_boolean());
	PRINT(v.is_number());
	PRINT(v.is_object());
	PRINT(v.is_array());
	PRINT(v.is_string());
	PRINT(v);
	//auto found=it.find(key);
	//PRINT(found);
	nyi
}*/

ostream& operator<<(ostream& o,Webcast const&){
	return o<<"Webcast()";
}

ostream& operator<<(ostream& o,BAlliance const&){
	nyi
}

ostream& operator<<(ostream& o,Event_v2 const& a){
	o<<"Event_v2( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	EVENT_V2_ITEMS(X)
	#undef X
	return o<<")";
}

Event_v2 parse_event(json j2){
	Event_v2 ee;
	set<string> seen;
	for(auto it=j2.begin();it!=j2.end();++it){
		bool found=0;
		#define X(A,B) if(it.key()==""#B){\
			seen.insert(""#B);\
			found=1;\
			ee.B=pull_element(it,""#B,(A*)nullptr);\
		}
		EVENT_V2_ITEMS(X)
		#undef X
		if(!found){
			PRINT(it.key());
			PRINT(it.value());
			nyi
		}
	}

	const set<string> all_elements{
		#define X(A,B) ""#B,
		EVENT_V2_ITEMS(X)
		#undef X
	};

	auto unset=all_elements-seen;
	if(unset.size()){
		cout<<"Unset:"<<all_elements-seen<<"\n";
		for(auto a:unset){
			#define X(A,B) if(a==""#B) cout<<""#B<<"("<<""#A<<")\n";
			EVENT_V2_ITEMS(X)
			#undef X
		}
		nyi
	}
	return ee;
}

Recipient pull_element(Json_obj_it at,string,Recipient*){
	PRINT(at.key());
	PRINT(at.value());
	nyi
	//return Recipient_list{};
}

ostream& operator<<(ostream& o,Recipient const& a){
	o<<"Recipient( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	RECIPIENT_ITEMS(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Award_type a){
	#define X(A) if(a==Award_type::A) return o<<""#A;

	X(CHAIRMANS)
    X(WINNER)
    X(FINALIST)

    X(WOODIE_FLOWERS)
    X(DEANS_LIST)
    X(VOLUNTEER)
    X(FOUNDERS)
    X(BART_KAMEN_MEMORIAL)
    X(MAKE_IT_LOUD)

    X(ENGINEERING_INSPIRATION )
    X(ROOKIE_ALL_STAR )
    X(GRACIOUS_PROFESSIONALISM )
    X(COOPERTITION )
    X(JUDGES )
    X(HIGHEST_ROOKIE_SEED)
    X(ROOKIE_INSPIRATION )
    X(INDUSTRIAL_DESIGN )
    X(QUALITY )
    X(SAFETY )
    X(SPORTSMANSHIP )
    X(CREATIVITY )
    X(ENGINEERING_EXCELLENCE )
    X(ENTREPRENEURSHIP )
    X(EXCELLENCE_IN_DESIGN )
    X(EXCELLENCE_IN_DESIGN_CAD )
    X(EXCELLENCE_IN_DESIGN_ANIMATION )
    X(DRIVING_TOMORROWS_TECHNOLOGY)
    X(IMAGERY )
    X(MEDIA_AND_TECHNOLOGY )
    X(INNOVATION_IN_CONTROL)
    X(SPIRIT )
    X(WEBSITE )
    X(VISUALIZATION)
    X(AUTODESK_INVENTOR )
    X(FUTURE_INNOVATOR )
    X(RECOGNITION_OF_EXTRAORDINARY_SERVICE )
    X(OUTSTANDING_CART )
    X(WSU_AIM_HIGHER )
    X(LEADERSHIP_IN_CONTROL)
    X(NUM_1_SEED )
    X(INCREDIBLE_PLAY)
    X(PEOPLES_CHOICE_ANIMATION)
    X(VISUALIZATION_RISING_STAR)
    X(BEST_OFFENSIVE_ROUND)
    X(BEST_PLAY_OF_THE_DAY)
    X(FEATHERWEIGHT_IN_THE_FINALS)
    X(MOST_PHOTOGENIC)
    X(OUTSTANDING_DEFENSE)
    X(POWER_TO_SIMPLIFY)
    X(AGAINST_ALL_ODDS)
    X(RISING_STAR)
    X(CHAIRMANS_HONORABLE_MENTION)
    X(CONTENT_COMMUNICATION_HONORABLE_MENTION)
    X(TECHNICAL_EXECUTION_HONORABLE_MENTION)
    X(REALIZATION)
    X(REALIZATION_HONORABLE_MENTION)
    X(DESIGN_YOUR_FUTURE)
    X(DESIGN_YOUR_FUTURE_HONORABLE_MENTION)
    X(SPECIAL_RECOGNITION_CHARACTER_ANIMATION)
    X(HIGH_SCORE)
    X(TEACHER_PIONEER)
    X(BEST_CRAFTSMANSHIP)
    X(BEST_DEFENSIVE_MATCH)
    X(PLAY_OF_THE_DAY)
    X(PROGRAMMING)
    X(PROFESSIONALISM)
    X(GOLDEN_CORNDOG)
    X(MOST_IMPROVED_TEAM)
	X(WILDCARD)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Award const& a){
	o<<"Award( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	AWARD_ITEMS(X)
	#undef X
	return o<<")";
}

Award_type pull_element(Json_obj_it j,string name,Award_type*){
	auto i=pull_element(j,name,(int*)0);
	return (Award_type)i;
}

Award parse_award(json j){
	Award r;
	for(auto at=j.begin();at!=j.end();++at){
		#define X(A,B) if(at.key()==""#B) r.B=pull_element(at,""#B,(A*)nullptr);
		AWARD_ITEMS(X)
		#undef X
	}
	return r;
}

std::vector<Award> calc_awards(string const& event_key){
	auto url="https://www.thebluealliance.com/api/v2/event/"+event_key+"/awards";
	auto data=scrape_cached(url);
	auto j=json::parse(data);
	return mapf(parse_award,j);
}

std::vector<Award> get_awards(string event_key){
	static map<string,vector<Award>> cache;
	auto f=cache.find(event_key);
	if(f!=cache.end()){
		return f->second;
	}
	return cache[event_key]=calc_awards(event_key);
}

vector<Event_v2> get_team_history(int team){
	auto url="http://www.thebluealliance.com/api/v2/team/frc"+as_string(team)+"/history/events";
	auto data=scrape_cached(url);
	auto j=json::parse(data);
	return mapf(parse_event,j);
}

vector<Event_v2> get_events_v2(int year){
	string url="http://www.thebluealliance.com/api/v2/events/"+as_string(year);
	auto data=scrape_cached(url);
	//cout<<data<<"\n";
	auto j=json::parse(data);
	return mapf(parse_event,j);
}

//will return the keys
vector<Event_v2> get_events(int year){
	return get_events_v2(year);
	nyi
	//if wanted to do this the super-clean way, would have the parsing function seperate from the downloading.  
	string url="http://www.thebluealliance.com/api/v1/events/list?year="+as_string(year);
	nyi/*json_spirit::Value value;
	auto data=scrape_cached(url);
	read(data,value);
	//cout<<value<<"\n";
	vector<BEvent> r;
	for(auto a:value.get_array()){
		BEvent b;
		auto obj=a.get_obj();
		//cout<<obj;
		for(auto elem:obj){
			//cout<<elem<<"\n";
			auto n=elem.name_;
			auto val=elem.value_;
			auto v=[&](){
				try{
					return val.get_str();
				}catch(...){
					cout<<"Expected string!"<<val<<"\n";
					throw;
				}
			};
			auto maybe_str=[&](){
				if(val.type()==json_spirit::null_type) return Maybe<string>();
				return Maybe<string>(v());
			};
			if(n=="name"){
				b.name=v();
			}else if(n=="end_date"){
				b.end_date=parse_date(maybe_str());
			}else if(n=="official"){
				switch(val.type()){
					case json_spirit::bool_type:
						b.official=val.get_bool();
						break;
					case json_spirit::null_type:
						b.official=0;
						break;
					default:
						nyi
				}
			}else if(n=="short_name"){
				b.name_short=maybe_str();
			}else if(n=="key"){
				b.key=v();
			}else if(n=="start_date"){
				b.start_date=parse_date(maybe_str());
			}else{
				cout<<"Got "<<n<<"\n";
				nyi
			}
		}
		//cout<<"b!\n";
		r|=b;
	}
	return r;*/
}

vector<string> get_event_keys(int year){
	vector<string> r;
	for(auto a:get_events(year)){
		r|=a.key;
	}
	//cout<<"got event ekys\n";
	return r;
}

ostream& operator<<(ostream& o,BEvent_details const& b){
	o<<"benent("<<b.name_short<<" ";
	o<<b.location;
	#define X(name) o<<""#name<<":"<<b.name<<" ";
	X(key)
	X(year)
	X(start_date)
	X(name)
	X(teams)
	X(official)
	X(matches)
	X(event_code)
	X(end_date)
	X(facebook_eid)
	#undef X
	o<<")";
	return o;
}

BEvent_details get_details(string const& event_code){
	auto s=scrape_cached("http://www.thebluealliance.com/api/v1/event/details?event="+event_code);
	//cout<<s<<"\n";
	auto j=json::parse(s);
	PRINT(j);
	nyi/*json_spirit::Value value;
	read(s,value);
	BEvent_details r;
	//cout<<value<<"\n";
	for(auto p:value.get_obj()){
		//cout<<"p="<<p<<"\n";
		auto n=p.name_;
		auto v=p.value_;

		auto s=[&](){
			try{
				return v.get_str();
			}catch(...){
				cout<<"Expected string, got:"<<v<<"\n";
				throw;
			}
		};
		auto ms=[&](){
			if(v.type()==json_spirit::null_type) return Maybe<string>();
			return Maybe<string>(s());
		};

		auto a=[&](){
			vector<string> r;
			for(auto elem:v.get_array()) r|=elem.get_str();
			return r;
		};
		if(n=="event_code"){
			r.event_code=s();
		}else if(n=="start_date"){
			r.start_date=parse_date(ms());
		}else if(n=="end_date"){
			r.end_date=parse_date(ms());
		}else if(n=="short_name"){
			if(v.type()!=json_spirit::null_type){
				r.name_short=s();
			}
		}else if(n=="facebook_eid"){
			//ignore for now
		}else if(n=="matches"){
			r.matches=a();
		}else if(n=="official"){
			switch(v.type()){
				case json_spirit::bool_type:
					r.official=v.get_bool();
					break;
				case json_spirit::null_type:
					r.official=0;
					break;
				default:
					nyi
			}
		}else if(n=="location"){
			if(v.type()!=json_spirit::null_type){
				r.location=s();
			}
		}else if(n=="teams"){
			r.teams=mapf(
				[](string s){ return Team(s); },
				filter(
					//I don't know why this even shows up in the data.  It isn't put in for any matches or anything.
					[](string s){ return s!="frc0"; },
					a()
				)
			);
		}else if(n=="key"){
			r.key=s();
		}else if(n=="year"){
			r.year=v.get_int();
		}else if(n=="name"){
			r.name=s();
		}else{
			cout<<"got "<<n<<"\n";
			nyi
		}
	}
	return r;*/
}

vector<BEvent_details> all_event_details(int year){
	return mapf(get_details,get_event_keys(year));
}

/*vector<string> get_array(json_spirit::Value const& v){
	vector<string> r;
	for(auto elem:v.get_array()){
		r|=elem.get_str();
	}
	return r;
}*/

template<typename A,typename B>
class Either{
	bool first_;
	A a_;
	B b_;

	public:
	Either(A a):first_(1),a_(a){}
	Either(B b):first_(0),b_(b){}

	bool first()const{ return first_; }
	operator bool()const{ return first_; }

	A const& a()const{
		assert(first_);
		return a;
	}

	B const& b()const{
		assert(!first_);
		return b;
	}
};

/*json_spirit::Value remove_array(json_spirit::Value v){
	auto a=v.get_array();
	assert(a.size()==1);
	return a[0];
}*/

//example input: 2010cmp_f1m1
Match_info match_info(string const& match_key){
	//cout<<"m="<<match_key<<"\n";
	//cout<<"going to ...\n";
	auto s=scrape_cached("http://www.thebluealliance.com/api/v1/match/details?match="+match_key);
	nyi/*json_spirit::Value value;
	read(s,value);
	Match_info r;
	//cout<<"starting\n";
	//cout<<value<<"\n";
	//value=value.get_array()[0];
	value=remove_array(value);
	for(auto p:value.get_obj()){
		//cout<<"p1="<<p<<"\n";
		auto n=p.name_;
		auto v=p.value_;
		auto s=[&v](){ return v.get_str(); };
		if(n=="match_number"){
			r.match_number=v.get_int();
		}else if(n=="team_keys"){
			for(auto elem:v.get_array()){
				r.teams|=Team(elem.get_str());
			}
		}else if(n=="set_number"){
			r.set_number=v.get_int();
		}else if(n=="competition_level"){
			auto p=parse_competition_level(s());
			if(!p){
				cout<<"Invalid competition level:"<<s()<<"\n";
				nyi
			}
			r.competition_level=*p;
		}else if(n=="key"){
			r.key=s();
		}else if(n=="alliances"){
			for(auto p:v.get_obj()){
				auto color=p.name_;
				Match_info::Alliance alliance;
				for(auto p2:p.value_.get_obj()){
					//cout<<"p2="<<p2<<"\n";
					auto n2=p2.name_;
					auto v2=p2.value_;
					if(n2=="score"){
						//woo non-normalized data type!
						int x;
						switch(v2.type()){
							case json_spirit::int_type:
								x=v2.get_int();
								break;
							case json_spirit::str_type:
								//could try to use a conversion w/ error checking.
								x=atoi(v2.get_str().c_str());
								break;
							default:
								nyi
						}
						alliance.score=x;
					}else if(n2=="teams"){
						alliance.teams=mapf([](string s){ return Team(s); },get_array(v2));
					}else{
						nyi//cout<<p2<<"\n";
						nyi
					}
				}
				r.alliances[color]=alliance;
			}
		}else if(n=="event"){
			//cout<<"a1\n";
			r.event=s();
			//cout<<"a2\n";
		}else if(n=="videos"){
			for(auto a:v.get_array()){
				Video v;
				for(auto b:a.get_obj()){
					auto n2=b.name_;
					auto v2=b.value_.get_str();
					if(n2=="type"){
						v.type=v2;
					}else if(n2=="key"){
						v.key=v2;
					}else{
						nyi
					}
				}
				r.video|=v;
			}
		}else if(n=="time_string"){
			//do nothing - seems to be a formatted time
		}else if(n=="time"){
			//do nothing - seems to be a string that's a Unix time.
		}else{
			cout<<"got "<<n<<"\n";
			cout<<"value="<<v<<endl;
			nyi
		}
		//cout<<"d1\n";
	}
	//cout<<"done\n";
	if(!ok(r)){
		//cout<<"Not ok: "<<r<<"\n";
	}
	//assert(ok(r));
	return r;*/
}

//opr: columns=teams,rows=present on this alliance?, vector=allaince score
//ccwm: columns=teams,rows=(red?1:(blue?-1:0)), vector=red score-blue score

vector<Match> matches_with_team(Team,int/* year*/){
	vector<string> r;
	nyi
	return r;
}

bool substring(string const& haystack,string const& needle){
	return strstr(haystack.c_str(),needle.c_str())!=NULL;
}

template<typename T>
multiset<T> to_multiset(vector<T> const& v){
	multiset<T> r;
	for(auto elem:v){
		r.insert(elem);
	}
	return r;
}
template<typename T>
ostream& operator<<(ostream& o,multiset<T> const& m){
	o<<"{ ";
	for(auto e:m){
		o<<e<<" ";
	}
	return o<<"}";
}

multiset<Regional_type> regional_types(int year){
	return to_multiset(mapf(
		[](BEvent_details b){
			if(!b.official) return string("Unofficial");
			if(substring(b.name,"District")){
				return string("District");
			}else if(substring(b.name,"Regional")){
				return string("Regional");
			}else if(substring(b.name,"Division")){
				return string("Division");
			}else if(substring(b.name,"Championship")){
				//Michigan/Mid atlantic championships
				return string("Championship");
			}else{
				return b.name;
			}
		},
		all_event_details(year)
	));
}

vector<string> match_keys(int year){
	return flatten(mapf([](string s){ return get_details(s).matches; },get_event_keys(year)));
}

vector<Match_info> matches(int year){
	//this could be parellized without too much trouble.
	try{
		return mapf(match_info,match_keys(year));
	}catch(...){
		cout<<"failure in parsing match.\n";
		throw;
	}
}

vector<Match_info> matches(int year,Team team){
	return filter([=](Match_info m)->bool{ return contains(m.teams,team); },matches(year));
}

vector<Match_info::Alliance> alliances(int year,Team team){
	matches(year,team);
	nyi
}

template<typename Collection>
auto number(Collection const& v)->map<ELEMENT(v),unsigned>{
	enumerate(v);
	map<ELEMENT(v),unsigned> r;
	for(auto p:enumerate(v)){
		r[p.second]=p.first;
	}
	return r;
}

//pulled from random webpage
template<class T>
boost::numeric::ublas::matrix<T> InvertMatrix(const boost::numeric::ublas::matrix<T>& input)
{
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<T> inverse;
	matrix<T> A(input);

	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0){
		cout<<"res="<<res<<"\n";
		nyi //return false;
	}

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<T> (A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	return inverse;
}

void t(){
	using namespace boost::numeric::ublas;
	int year=2013;
	auto match_list=take(2,matches(year));
	auto alliance_list=alliances(match_list);
	auto team_list=teams(match_list);
	auto nums=number(team_list);
	matrix<double> m(team_list.size(),alliance_list.size()); //asume we've just got two sides.  
	for(unsigned i=0;i<team_list.size();i++){
		for(unsigned j=0;j<alliance_list.size();j++){
			m(i,j)=0;
		}
	}
	boost::numeric::ublas::vector<double> v(alliance_list.size());
	//need to declare the vector here...
	cout<<"nums="<<nums<<"\n";
	cout<<"match_list="<<match_list<<"\n";
	cout<<"alliance_list="<<alliance_list<<"\n";
	for(auto p:enumerate(alliance_list)){
		cout<<p<<"\n";
		for(auto team:p.second.teams){
			cout<<"t="<<team<<" "<<nums[team]<<"\n";
			m(nums[team],p.first)=1;
		}
		v[p.first]=p.second.score;
	}
	
	cout<<m<<"\n";
	cout<<InvertMatrix(m)<<"\n";
	cout<<trans(m)<<"\n";
	auto p=prod(trans(m),m);
	cout<<prod(m,trans(m))<<"\n";
	cout<<"vec="<<v<<"\n";
	cout<<prod(p,v)<<"\n";
	//inverse( trans(x)*x ) * trans(x) * y = X+ * y = Beta_hat
}

struct BTeam_basic{
	string key_name;
	int team_number;
	string name,nickname,website;
	vector<string> event_keys;
	string location;
	string locality,region,country_name;
};

ostream& operator<<(ostream& o,BTeam_basic const& b){
	o<<"BTeam_basic( ";
	#define X(name) o<<""#name<<"="<<b.name<<" ";
	X(key_name)
	X(team_number)
	X(name)
	X(nickname)
	X(website)
	X(event_keys)
	X(location)
	X(locality)
	X(region)
	X(country_name)
	#undef X
	return o<<")";
}

BTeam_basic team_basic(Team team){
	stringstream ss;
	ss<<"http://www.thebluealliance.com/api/v1/teams/show?teams="<<team;
	auto data=scrape_cached(ss.str());
	nyi/*json_spirit::Value value;
	read(data,value);
	//cout<<value<<"\n";
	for(auto a:value.get_array()){
		BTeam_basic r;
		auto obj=a.get_obj();
		for(auto elem:obj){
			auto n=elem.name_;
			auto v=elem.value_;
			auto s=[&](){ return v.get_str(); };
			if(n=="website"){
				r.website=s();
			}else if(n=="name"){
				r.name=s();
			}else if(n=="locality"){
				r.locality=s();
			}else if(n=="region"){
				r.region=s();
			}else if(n=="team_number"){
				r.team_number=v.get_int();
			}else if(n=="location"){
				r.location=s();
			}else if(n=="key"){
				r.key_name=s();
			}else if(n=="country_name"){
				r.country_name=s();
			}else if(n=="nickname"){
				r.nickname=s();
			}else if(n=="events"){
				//note that this contains only the events for the current year.
				for(auto elem:v.get_array()){
					r.event_keys|=elem.get_str();
				}
			}else{
				cout<<"n="<<n<<"\n";
				nyi
			}
		}
		return r;
	}
	nyi*/
}

struct BTeam_details{
	string key_name,team_number,nickname,website;
	vector<string> event_keys;
	string location,locality,country,region;
};

ostream& operator<<(ostream& o,BTeam_details const& b){
	o<<"BTeam_details( ";
	#define X(name) o<<""#name<<"="<<b.name<<" ";
	X(key_name)
	X(team_number)
	X(nickname)
	X(website)
	X(event_keys)
	X(location)
	X(locality)
	X(country)
	X(region)
	#undef X
	return o<<")";
}

//BTeam_details team_details(Team team)nyi

template<typename T>
struct Tag{
	string name;
	T body;//this just needs to be something that is convertible to a string.
};

template<typename T>
ostream& operator<<(ostream& o,Tag<T>)nyi

template<typename T>
Tag<T> tag2(string a,T b){ return Tag<T>{a,b}; }

template<typename T>
string operator+(string,Tag<T>)nyi

template<typename T>
string operator+(Tag<T>,string)nyi

string team_page(Team team){
	auto title=string("Team ")+as_string(team);
	return as_string(tag("html",
		tag("head",
			tag("title",title)
		)+
		tag("body",
			tag("h1",title)+
			"The body"+as_string(team_basic(team))
		)
	));
}

struct BAward{
	string event_key;
	string name;
	struct Recipient{
		Maybe<int> team_number;//things like "Volunteer of the Year" don't have to have a team.
		Maybe<string> awardee;
	};
	vector<Recipient> recipient_list;
	int year;
};

ostream& operator<<(ostream& o,BAward::Recipient a){
	o<<"BAward::Recipient(";
	o<<a.team_number;
	o<<" "<<a.awardee;
	return o<<")";
}

ostream& operator<<(ostream& o,BAward a){
	o<<"BAward(";
	#define X(name) o<<""#name<<":"<<a.name<<" ";
	X(event_key)
	X(name)
	X(recipient_list)
	X(year)
	return o<<")";
}

/*BAward::Recipient parse_recipient(json_spirit::Value value){
	BAward::Recipient r;
	for(auto a:value.get_obj()){
		auto k=a.name_;
		auto v=a.value_;
		if(k=="team_number"){
			if(v.type()!=json_spirit::null_type){
				r.team_number=v.get_int();
			}
		}else if(k=="awardee"){
			if(v.type()!=json_spirit::null_type){
				r.awardee=v.get_str();
			}
		}else{
			cout<<"k2="<<k<<"\n";
			nyi
		}
	}
	return r;
}*/

/*BAward parse_award(json_spirit::Value value){
	//cout<<value<<"\n";
	BAward r;
	for(auto a:value.get_obj()){
		auto k=a.name_;
		auto v=a.value_;
		auto s=[&](){
			try{
				return v.get_str();
			}catch(...){
				cout<<"k="<<k<<"\n";
				cout<<"v="<<v<<"\n";
				nyi
			}
		};

		if(k=="event_key"){
			r.event_key=s();
		}else if(k=="name"){
			r.name=s();
		}else if(k=="recipient_list"){
			try{
				for(auto elem:v.get_array()){
					r.recipient_list|=parse_recipient(elem);
				}
			}catch(...){
				cout<<value<<"\n";
				nyi
			}
		}else if(k=="year"){
			r.year=v.get_int();
		}else{
			cout<<"k1="<<k<<"\n";
			nyi
		}
	}
	return r;
}*/

//appears inside of the team details
struct BEvent_model{
	string key;
	Maybe<string> end_date;
	string name;
	//Maybe<string> facebook_eid;
	//matches - ignore for now.
	Maybe<string> short_name;
	vector<BAward> awards;
};

ostream& operator<<(ostream& o,BEvent_model a){
	o<<"BEvent_model(";
	#define X(name) o<<""#name<<":"<<a.name<<" ";
	X(key)
	X(end_date)
	X(name)
	X(short_name)
	X(awards)
	#undef X
	return o<<")";
}

/*BEvent_model parse_event(json_spirit::Value value){
	//cout<<value<<"\n";
	//nyi
	BEvent_model r;
	for(auto p:value.get_obj()){
		auto k=p.name_;
		auto v=p.value_;
		//cout<<"Event n"<<k<<"\n";
		auto s=[&](){
			try{
				return v.get_str();
			}catch(...){
				cout<<"Failed at "<<k<<"\n";
				cout<<"Value:"<<v<<"\n";
				throw;
			}
		};
		auto ms=[&](){
			if(v.type()==json_spirit::null_type) return Maybe<string>();
			return Maybe<string>(v.get_str());
		};

		if(k=="key"){
			r.key=s();
		}else if(k=="end_date"){
			r.end_date=ms();
		}else if(k=="name"){
			r.name=s();
		}else if(k=="short_name"){
			r.short_name=ms();
		}else if(k=="facebook_eid"){
			//skip
		}else if(k=="matches"){
			//skip
		}else if(k=="official"){
			//skip
		}else if(k=="awards"){
			for(auto elem:v.get_array()){
				try{
					r.awards|=parse_award(elem);
				}catch(...){
					nyi
				}
			}
		}else if(k=="location"){
			//skip
		}else if(k=="event_code"){
			//how is this different from "key"?
			//skip
		}else if(k=="year"){
			//skip
		}else if(k=="event_type_string"){
			//skip
		}else if(k=="start_date"){
			//skip
		}else if(k=="event_type"){
			//skip
		}else{
			cout<<"k="<<k<<"\n";
			nyi
		}
	}
	return r;
}*/

struct BTeam_data{
	Maybe<string> website;
	Maybe<string> name;
	Maybe<string> locality;
	string region;
	//string country; exists in the API document
	Maybe<string> location;
	string team_number;
	string key;
	Maybe<string> nickname;
	vector<BEvent_model> events;

	//doesn't exist in the API document
	Maybe<string> country_name;
};

ostream& operator<<(ostream& o,BTeam_data t){
	o<<"Team_data(";
	#define X(name) o<<""#name<<":"<<t.name<<" ";
	X(website)
	X(name)
	X(locality)
	X(region)
	//X(country)
	X(location)
	X(team_number)
	X(key)
	X(nickname)
	X(events)
	X(country_name)
	return o<<")";
}

BTeam_data parse_team_data(string page){
	nyi/*json_spirit::Value value;
	read(page,value);
	BTeam_data r;
	//cout<<"start\n";
	for(auto a:value.get_obj()){
		//cout<<"got:"<<a<<"\n";
		//cout<<"name:"<<a.name_<<"\n";
		//cout<<"value:"<<a.value_;
		auto s=[&](){
			try{
				return a.value_.get_str();
			}catch(...){
				cout<<value<<"\n";
				cout<<"Not a str ("<<a.name_<<"):"<<a.value_<<"\n";
				throw;
			}
		};
		auto ms=[&](){
			if(a.value_.type()==json_spirit::null_type) return Maybe<string>();
			return Maybe<string>(s());
		};
		if(a.name_=="website"){
			r.website=ms();
		}else if(a.name_=="name"){
			r.name=ms();
		}else if(a.name_=="locality"){
			r.locality=ms();
		}else if(a.name_=="region"){
			r.region=ms();
		//}else if(a.name_=="country"){
		//	r.country=a.value_.get_st
		}else if(a.name_=="location"){
			r.location=ms();
		}else if(a.name_=="team_number"){
			r.team_number=a.value_.get_int();
		}else if(a.name_=="key"){
			r.key=a.value_.get_str();
		}else if(a.name_=="country_name"){
			r.country_name=ms();
		}else if(a.name_=="nickname"){
			r.nickname=ms();
		}else if(a.name_=="events"){
			for(auto elem:a.value_.get_array()){
				try{
					r.events|=parse_event(elem);
				}catch(...){
					nyi
				}
			}
		}else{
			//cout<<r<<"\n";
			cout<<"name="<<a.name_<<"\n";
			//cout<<a<<"\n";
			nyi
		}
	}
	return r;*/
}

BTeam_data team_data(Team team,int year){
	string s="http://www.thebluealliance.com/api/v2/team/frc"+as_string(team)+"/"+as_string(year);
	auto s2=scrape_cached(s);
	try{
		return parse_team_data(s2);
	}catch(...){
		cout<<"Team:"<<team<<" "<<year<<"\n";
		cout<<"s2=\""<<s2<<"\"\n";
		cout<<"Error parsing 3\n";
		throw;
	}
}

template<typename T>
vector<T> without_last(vector<T> v){
	v.pop_back();
	return v;
}

vector<string> simplify_award_name(vector<string> sp){
	assert(sp.size());

	if(sp[0]=="Event"){
		return simplify_award_name(tail(sp));
	}

	static const vector<string> remove_from_end{"Award","(#1)","Friday","Saturday","-"};
	for(auto a:remove_from_end){
		if(sp[sp.size()-1]==a){
			return simplify_award_name(without_last(sp));
		}
	}

	vector<string> divisions={"Archimedes","Curie","Galileo","Newton"};

	if(contains(divisions,sp[sp.size()-1])){
		sp.pop_back();//remove division name
		assert(sp.size());
		sp.pop_back();//remove hyphen
		return simplify_award_name(sp);
	}

	if(sp[0]=="Championship" || contains(divisions,sp[0]) || sp[0]=="Division" || sp[0]=="District" || sp[0]=="Regional"){
		return simplify_award_name(tail(sp));
	}

	if(sp[0]=="-") return simplify_award_name(tail(sp));

	if(
		sp[0]=="Autodesk" || sp[0]=="DaimlerChrysler" || sp[0]=="Chrysler" || sp[0]=="GM" || sp[0]=="RadioShack" || sp[0]=="Xerox" || sp[0]=="Delphi" || sp[0]=="J&J" || sp[0]=="Motorola"
	){
		return simplify_award_name(tail(sp));
	}
	if(
		(sp[0]=="General" && sp[1]=="Motors") || 
		(sp[0]=="Underwriters" && (sp[1]=="Laboratory" || sp[1]=="Laboratories")) ||
		(sp[0]=="Radio" && sp[1]=="Shack") ||
		(sp[0]=="Rockwell" && sp[1]=="Automation") ||
		(sp[0]=="State" && sp[1]=="Championship")
	){
		return simplify_award_name(tail(tail(sp)));
	}

	if(sp[0]=="Johnson" && sp[1]=="&" && sp[2]=="Johnson"){
		return simplify_award_name(skip(3,sp));
	}

	for(unsigned i=0;i+1<sp.size();i++){
		if(sp[i]=="sponsored" && sp[i+1]=="by"){
			sp=take(i-1,sp);
		}
	}

	return sp;
}

string without_odd_chars(string s){
	stringstream ss;
	for(auto c:s){
		if(isprint(c)) ss<<c;
	}
	return ss.str();
}

string simplify_award_name(string s){
	s=without_odd_chars(s);
	if(strstr(s.c_str(),"Kleiner")) return "Entrepreneurship";
	auto sp=split(s);
	//cout<<"before:"<<s<<"\n";
	sp=simplify_award_name(sp);
	s=join(sp,' ');
	//cout<<"after:"<<s<<"\n";

	//TODO: Need to add some unification of Dean's list stuff.

	if(s=="Website Design") return "Website";
	if(s=="Chairmans" || s=="Chairman's Award Winner" || s=="Chairmans Winner") return "Chairman's";
	if(s=="Judges") return "Judge's";
	if(s=="Finalists") return "Finalist";
	if(s=="Winners") return "Winner";
	if(s=="Web Site") return "Website";
	if(s=="Leadership in Controls") return "Leadership in Control";
	if(s=="\"Driving Tommorow's Technology\"" || s=="\"Driving Tomorrows Technology") return "\"Driving Tomorrow's Technology\"";
	if(strstr(s.c_str(),"Imagery")) return "Imagery";
	return s;
}

typedef string Event_key;
vector<pair<Event_key,string>> awards(Team team,int year){
	vector<pair<Event_key,string>> r;
	for(auto event:team_data(team,year).events){
		for(auto award:event.awards){
			r|=make_pair(event.key,simplify_award_name(award.name));
		}
	}
	return r;
}

vector<int> all_years(){
	vector<int> r;
	//todo: change this back to 1992.
	for(int i=1992;i<=2014;i++){
		r|=i;
	}
	return r;
}

vector<Match_info> all_matches(){
	vector<Match_info> r;
	for(auto y:all_years()) r|=matches(y);
	return r;
}

set<Team> calc_all_teams(){
	auto t=teams(all_matches());
	//t.erase(Team("frc764"));//team page for this one doesn't exits right in 1993.
	return t;
}

set<Team> all_teams(){
	static const auto r=calc_all_teams();
	return r;
}

void reseed(){
     struct timeval time; 
     gettimeofday(&time,NULL);

     // microsecond has 1 000 000
     // Assuming you did not need quite that accuracy
     // Also do not assume the system clock has that accuracy.
     srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

vector<string> calc_awards(int year){
	vector<string> r;
	for(auto team:all_teams()){
		for(auto award:awards(team,year)){
			r|=award.second;
		}
	}
	return r;
}

vector<string> awards(int year){
	static map<int,vector<string>> cache;
	auto f=cache.find(year);
	if(f==cache.end()){
		return cache[year]=calc_awards(year);
	}
	return f->second;
}

vector<string> all_awards(){
	vector<string> r;
	for(auto year:all_years()){
		r|=awards(year);
		//cout<<"r="<<r<<"\n";
	}
	return r;
}

void award_appearances(){
	//figure out how many times each award was given in each year
	for(auto award:to_set(all_awards())){
		cout<<"Award: \""<<award<<"\"\n";
		for(auto year:all_years()){
			cout<<year<<":";
			cout<<count(awards(year))[award]<<"\n";
		}
	}
}

void awards(){
	reseed();
	//cout<<p<<"\n";
/*	auto p=team_data(
	for(auto a:p.events){
		cout<<a.awards<<"\n";
	}*/
	//cout<<count(all_awards())<<"\n";
	award_appearances();
	return;

	auto c=count(all_awards());
	auto d=sorted(reverse_pairs(map_to_pairs(c)));
	for(auto a:d){
		cout<<a<<"\n";
	}
	cout<<"demo:\n";
	for(unsigned i=2004;i<2014;i++){
		auto a=awards(Team("frc1425"),i);
		for(auto e:a) cout<<e<<"\n";
	}
	//return 0;
}
