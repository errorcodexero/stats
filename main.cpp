#include "str.h"
#include "util.h"
#include "set.h"
#include "scrape.h"
#include "map.h"
#include "team.h"

using namespace std;

//should probably move to util
map<string,string> env_vars(char **envp){
	assert(envp);
	map<string,string> r;
	for(char **env=envp;*env;env++){
		string e=*env;
		auto sp=split(e,'=');
		r[sp[0]]=join(tail(sp),'=');
	}
	return r;
}

//should probably move to util
vector<string> args(int argc,char **argv){
	vector<string> r;
	for(unsigned i=1;i<argc;i++) r|=string(argv[i]);
	return r;
}

enum class Listing_type{TEAM,EVENT,MATCH};

//this might change to a map to a vector of string at some point.
map<string,vector<string>> get_flags(vector<string> args){
	auto flag=mapf(
		[](string s)->Maybe<string>{
			if(prefix(s,"--")){
				return string(s.c_str()+2);
			}else{
				return Maybe<string>();
			}
		},
		args
	);
	flag|=Maybe<string>("Ignore");
	map<string,vector<string>> r;
	for(unsigned i=0;i<args.size();i++){
		if(flag[i]){
			auto name=*flag[i];
			if(flag[i+1]){
				r[name];
			}else{
				r[name]|=args[i+1];
				i++;
			}
		}else{
			//probably should just go into an argument list...
			nyi
		}
	}
	return r;
}

set<string> events(vector<Match_info> const& v){
	set<string> r;
	for(auto a:v) r|=a.event;
	return r;
}

int run_main(map<string,vector<string>> flags){
	set<string> flags_used;
	auto get_flag=[&](string name)->Maybe<vector<string>>{
		auto f=flags.find(name);
		if(f==flags.end()) return Maybe<vector<string>>();
		flags_used|=name;
		return Maybe<vector<string>>(f->second);
	};
	auto get_flag_default=[&](string name,string default_value)->vector<string>{
		auto f=get_flag(name);
		if(!f) return vector<string>{default_value};
		return *f;
	};

	auto y=get_flag_default("year","2013");
	vector<Match_info> m;
	for(auto year_str:y){
		int year=atoi(year_str.c_str());
		m|=matches(year);
	}

	auto b=get_flag("team");
	if(b){
		for(auto f:*b){
			Team team{f};
			m=filter([&](Match_info const& m){ return teams(m)&team; },m);
		}
	}

	auto c=get_flag("competition_level");
	if(c){
		for(auto f:*c){
			auto p=parse_competition_level(f);
			if(!p){
				cout<<"Error: Could not parse to a competition level: \""<<f<<"\"\n";
				return 1;
			}
			auto level=*p;
			m=filter([&](Match_info const& m){ return m.competition_level==level; },m);
		}
	}

	auto d=get_flag("event");
	if(d){
		for(auto f:*d){
			m=filter([&](Match_info const& m){ return m.event==f; },m);
		}
	}

	if(get_flag("matches")){
		//cout<<m<<"\n";
		for(auto a:m){
			cout<<a<<"\n";
		}
	}

	if(get_flag("averages")){
		cout<<"mean="<<mean(scores(m))<<"\n";
		cout<<"median="<<median(scores(m))<<"\n";
		cout<<"mode="<<mode(scores(m))<<"\n";
		cout<<"Mean given won="<<mean_score(winning_alliances(m))<<"\n";
		cout<<"Mean given lost="<<mean_score(losing_alliances(m))<<"\n";
	}
	if(get_flag("teams")){
		cout<<teams(m)<<"\n";
	}
	if(get_flag("team_count")){
		cout<<"Teams: "<<teams(m).size()<<"\n";
	}
	if(get_flag("events")){
		cout<<events(m)<<"\n";
	}
	if(get_flag("event_count")){
		cout<<"Events: "<<events(m).size()<<"\n";
	}
	if(get_flag("competition_levels")){
		cout<<competition_level(m)<<"\n";
	}
	auto unused=a_and_not_b(keys(flags),flags_used);
	if(unused.size()){
		cout<<"Error: Unused flags:"<<unused<<"\n";
		return 1;
	}

	return 0;
}

//takes about 5 minutes to get all the details on the events, then extra time beyond that to get the matches.
int main(int argc,char **argv,char **envp){
	auto env=env_vars(envp);
	auto f=env.find("QUERY");
	if(f!=env.end()){ //this is for if you want to run the program as a cgi script
		map<string,vector<string>> flags;
		mapf(
			[&flags](string s){
				auto sp=split(s,'=');
				if(sp.size()==1){
					flags[sp[0]]=vector<string>();
					return 0;
				}
				if(sp.size()==2){
					flags[sp[0]]|=sp[1];
				}
				nyi //should probably just join all the other things back together.
			},
			split(f->second,'&')
		);
		cout<<"Content-type: text/plain\n\n";
		return run_main(flags);
	}
	return run_main(get_flags(args(argc,argv)));

	//cout<<team_basic(Team("frc1425"))<<"\n";
	//t();
	//return 0;

	/*int year=2013;
	auto m=matches(year);
	//cout<<alliances(m).size()<<"\n";
	cout<<"mean="<<mean(scores(m))<<"\n";
	cout<<"median="<<median(scores(m))<<"\n";
	cout<<"mode="<<mode(scores(m))<<"\n";
	cout<<"Mean given won="<<mean_score(winning_alliances(m))<<"\n";
	cout<<"Mean given lost="<<mean_score(losing_alliances(m))<<"\n";
	return 0;
	cout<<regional_types(2012)<<"\n";
	cout<<regional_types(2013)<<"\n";
	//cout<<matches(2013)<<"\n";
	for(auto elem:matches(2012,Team("frc1425"))){
		cout<<elem<<"\n";
	}
//	match_info();
	return 0;

	auto keys=get_event_keys(2013);
	cout<<"got keys\n";
	for(auto key:keys){
		auto det=get_details(key);
		cout<<det<<"\n";
		for(auto match:det.matches){
			cout<<"match="<<match<<"\n";
			cout<<"mi="<<match_info(match)<<"\n";
		}
		cout<<"here3\n";
	}
	//todo: some basic sanity checks, like the team list looks like the set of teams in the matches.
	return 0;
	//cout<<get_events();
	return 0;

	//cout<<split_on("this is that thing.","t");
	//return 0;
	cout<<events()<<"\n";
	for(auto event:events()){
		cout<<event<<" "<<scrape_cached(event.url);
	}
	//get_details
	return 0;*/
}
