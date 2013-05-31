#include<sstream>
#include<cassert>
#include<string.h>
#include<boost/numeric/ublas/matrix.hpp>
#include<boost/numeric/ublas/vector.hpp>
#include<boost/numeric/ublas/lu.hpp>
#include<boost/numeric/ublas/io.hpp>
#include "json_spirit.h"
#include "util.h"
#include "str.h"
#include "map.h"
#include "set.h"
#include "team.h"
#include "bevent.h"
#include "match_info.h"

using namespace std;

string escape_to_commandline(char c){
	//NOTE: If you ever want to run this as a cgi script or something, you will neeed to make this better.
	stringstream ss;
	if(c=='&' || c=='-' || c==';') ss<<"\\";
	ss<<c;
	return ss.str();
}

//remove spaces at the end of the string
string rstrip(string const& s){
	int i=0;
	while(s[s.size()-1-i]==' ') i++;
	return s.substr(0,s.size()-i);
}

string escape_to_commandline(string const& s){
	stringstream ss;
	for(auto c:s) ss<<escape_to_commandline(c);
	return ss.str();
}

//going to make a cache of downloaded pages...
void download_page(string const& url,string const& outfile){
	static const string tmpfile="tmp1.tmp";
	stringstream ss;
	//moves the file into place when it's done instead of downloading it to the right place because otherwise if it gets killed halfway through you can end up with an invalid file.
	ss<<"wget -q -O "<<tmpfile<<" "<<escape_to_commandline(url);
	ss<<" && mv "<<tmpfile<<" "<<outfile;
	//cout<<"Running: "<<ss.str()<<"\n";
	auto r=system(ss.str().c_str());
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
	return slurp(cache_file);
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

ostream& operator<<(ostream& o,json_spirit::Value const& v){
	write(v,o,json_spirit::pretty_print);
	return o;
}

ostream& operator<<(ostream& o,json_spirit::Object const& obj){
	write(obj,o,json_spirit::pretty_print);
	return o;
}

template<typename A>
ostream& operator<<(ostream& o,json_spirit::Pair_impl<A> const& p){
	//write(p,o,json_spirit::pretty_print);
	return o<<"("<<p.name_<<" "<<p.value_<<")";
}

//will return the keys
vector<BEvent> get_events(int year){
	//if wanted to do this the super-clean way, would have the parsing function seperate from the downloading.  
	string url="http://www.thebluealliance.com/api/v1/events/list?year="+as_string(year);
	json_spirit::Value value;
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
			auto v=[&](){ return val.get_str(); };
			if(n=="name"){
				b.name=v(); //r|=elem.value_.get_str();
			}else if(n=="end_date"){
				b.end_date=v();
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
				//b.official=v.get_bool();
			}else if(n=="short_name"){
				if(val.type()==json_spirit::null_type) continue;
				b.name_short=v();
			}else if(n=="key"){
				b.key=v();
			}else if(n=="start_date"){
				b.start_date=v();
			}else{
				cout<<"Got "<<n<<"\n";
				nyi
			}
		}
		//cout<<"b!\n";
		r|=b;
	}
	return r;
}

vector<string> get_event_keys(int year){
	vector<string> r;
	for(auto a:get_events(year)){
		r|=a.key;
	}
	return r;
}

//data from the event details page of the blue alliance
struct BEvent_details{
	Maybe<string> location;
	string key;
	string year;//could actually make this an int
	string start_date,name;
	vector<Team> teams;
	bool official;
	vector<string> matches;
	string event_code;
	string end_date;
	Maybe<string> name_short;
	Maybe<bool> facebook_eid; //fix the type of this.
};

ostream& operator<<(ostream& o,BEvent_details const& b){
	return o<<"benent("<<b.name_short<<" "<<b.location<<")";
}

BEvent_details get_details(string const& event_code){
	auto s=scrape_cached("http://www.thebluealliance.com/api/v1/event/details?event="+event_code);
	//cout<<s<<"\n";
	json_spirit::Value value;
	read(s,value);
	BEvent_details r;
	//cout<<value<<"\n";
	for(auto p:value.get_obj()){
		//cout<<"p="<<p<<"\n";
		auto n=p.name_;
		auto v=p.value_;
		auto s=[&](){ return v.get_str(); };
		auto a=[&](){
			vector<string> r;
			for(auto elem:v.get_array()) r|=elem.get_str();
			return r;
		};
		if(n=="event_code"){
			r.event_code=s();
		}else if(n=="start_date"){
			r.start_date=s();
		}else if(n=="end_date"){
			r.end_date=s();
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
	return r;
}

vector<BEvent_details> all_event_details(int year){
	return mapf(get_details,get_event_keys(year));
}

vector<string> get_array(json_spirit::Value const& v){
	vector<string> r;
	for(auto elem:v.get_array()){
		r|=elem.get_str();
	}
	return r;
}

//example input: 2010cmp_f1m1
Match_info match_info(string const& match_key){
	//cout<<"going to ...\n";
	auto s=scrape_cached("http://www.thebluealliance.com/api/v1/match/details?match="+match_key);
	json_spirit::Value value;
	read(s,value);
	Match_info r;
	//cout<<"starting\n";
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
			r.competition_level=*parse_competition_level(s());
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
						cout<<p2<<"\n";
						nyi
					}
				}
				r.alliances[color]=alliance;
			}
		}else if(n=="event"){
			//cout<<"a1\n";
			r.event=s();
			//cout<<"a2\n";
		}else{
			cout<<"got "<<n<<"\n";
			nyi
		}
		//cout<<"d1\n";
	}
	//cout<<"done\n";
	if(!ok(r)){
		//cout<<"Not ok: "<<r<<"\n";
	}
	//assert(ok(r));
	return r;
}

//opr: columns=teams,rows=present on this alliance?, vector=allaince score
//ccwm: columns=teams,rows=(red?1:(blue?-1:0)), vector=red score-blue score

vector<Match> matches_with_team(Team team,int year){
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
	return mapf(match_info,match_keys(year));
}

vector<Match_info> matches(int year,Team team){
	return filter([=](Match_info m)->bool{ return contains(m.teams,team); },matches(year));
}

vector<Match_info::Alliance> alliances(int year,Team team){
	matches(year,team);
	nyi
}

#define RM_REF(X) typename remove_reference<X>::type
#define RM_CONST(X) typename remove_const<X>::type
#define ELEMENT(X) RM_CONST(RM_REF(decltype(*begin(X))))

template<typename Collection>
auto enumerate(Collection const& v)->vector<pair<unsigned,ELEMENT(v)>>{
	vector<pair<unsigned,ELEMENT(v)>> r;
	unsigned i=0;
	for(auto e:v){
		r.push_back(make_pair(i,e));
		i++;
	}
	return r;
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
	json_spirit::Value value;
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
	nyi
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

BTeam_details team_details(Team team)nyi

//if wanted to be fancier, would actually make this be a structure and then have a way to turn it into a string.
string tag(string name,string body){
	stringstream ss;
	ss<<"<"<<name<<">"<<body<<"</"<<name<<">";
	return ss.str();
}

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

