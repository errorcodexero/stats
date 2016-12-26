#include "team_list.h"
#include "../util.h"
#include "../scrape.h"
#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;

vector<unsigned> range(unsigned lim){
	vector<unsigned> r;
	for(unsigned i=0;i<lim;i++) r|=i;
	return r;
}

ostream& operator<<(ostream& o,Team_info const& a){
	o<<"Team_info( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	TEAM_INFO_ITEMS(X)
	#undef X
	return o<<")";
}

Team_info interpret_team(json const& j){
	Team_info r;
	set<string> used;
	for(auto at=j.begin();at!=j.end();++at){
		bool found=0;
		#define X(A,B) if(at.key()==""#B){ \
			found=1; \
			used.insert(""#B); \
			r.B=pull_element(at,""#B,(A*)0);\
		}
		TEAM_INFO_ITEMS(X)
		#undef X
		if(!found){
			nyi
		}
	}
	const set<string> all_items{
		#define X(A,B) ""#B,
		TEAM_INFO_ITEMS(X)
		#undef X
	};
	auto unused=all_items-used;
	if(unused.size()){
		nyi
	}
	return r;
}

vector<Team_info> teams_page(int page){
	auto s=scrape_cached("http://www.thebluealliance.com/api/v2/teams/"+as_string(page));
	json j=json::parse(s);
	assert(j.is_array());
	return mapf(interpret_team,j);
}

vector<Team_info> all_team_details(){
	return flatten(mapf(teams_page,range(20)));
}

auto calc_event_teams(string const& event_key){
	auto s=scrape_cached("http://www.thebluealliance.com/api/v2/event/"+event_key+"/teams");
	return mapf(interpret_team,json::parse(s));
}

vector<Team_info> const& event_teams(string const& event_key){
	static map<string,vector<Team_info>> cache;
	auto f=cache.find(event_key);
	if(f==cache.end()){
		return cache[event_key]=calc_event_teams(event_key);
	}
	return f->second;
}

map<Year,string> team_history_districts(string const& team_key){
	auto s=scrape_cached("https://www.thebluealliance.com/api/v2/team/"+team_key+"/history/districts");
	auto j=json::parse(s);
	map<Year,string> r;
	for(auto at=j.begin();at!=j.end();++at){
		r[as_type(at.key(),(int*)0)]=at.value();
	}
	return r;
}
