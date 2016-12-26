#include "scrape_team.h"
#include "util.h"
#include "team.h"
#include "scrape.h"
//#include "json_spirit.h"
#include "str.h"
#include "set.h"
#include "main.h"
#include "tuple.h"

using namespace std;

Team_info::Team_info(Team t):team(t){}

ostream& operator<<(ostream& o,Team_info const& a){
	o<<"Team_info("<<a.team<<" ";
	//o<<" "<<a.website;
	//o<<a.name<<" ";
	o<<a.nickname<<" ";
	o<<a.locality<<" "<<a.region<<" ";
	//country,location,team_number,
	//o<<a.key;
	//,events
	return o<<")";
}

/*template<typename T>
ostream& operator<<(ostream& o,json_spirit::Value_impl<T> j){
	switch(j.type()){
		case 0:return o<<j.get_obj();
		case 1:return o<<j.get_array();
		case 2:return o<<j.get_str();
		case 3:return o<<j.get_bool();
		case 4:return o<<j.get_int();
		case 5:return o<<j.get_real();
		case 6:
			nyi//write(j,o,json_spirit::pretty_print);
			return o;
		default:
			o<<"value="<<j.type();
	}
	return o;
}*/

/*template<typename T>
ostream& operator<<(ostream& o,json_spirit::Pair_impl<T> p){
	o<<"("<<p.name_<<":"<<p.value_<<")";
	return o;
}*/

//www.thebluealliance.com/api/v2/team/<team key>/(<year>)
Team_info get(Team team,Year year){
	auto url=[=](){
		stringstream ss;
		ss<<"http://www.thebluealliance.com/api/v2/team/frc"<<team<<"/"<<year;
		return ss.str();
	}();
	nyi
	#if 0
	json_spirit::Value value;
	auto data=scrape_cached(url);
	read(data,value);

	Team_info r(team);	
	//string website,name,locality,region,team,country,location,team_number,key,nickname,events;
	for(auto a:value.get_obj()){
		auto n=a.name_;
		auto val=a.value_;
		auto s=[=](){
			try{
				return val.get_str();
			}catch(...){
				cout<<"Error on "<<n<<":"<<val<<endl;
				throw;
			}
		};
		auto ms=[=](){
			if(val.type()==6){
				return Maybe<string>();
			}
			return Maybe<string>(s());
		};
		if(n=="locality"){
			r.locality=ms();
		}else if(n=="website"){
			r.website=ms();
		}else if(n=="name"){
			r.name=ms();
		}else if(n=="region"){
			r.region=ms();
		}else if(n=="team_number"){
			Team t(val.get_int());
			if(t!=team){
				cout<<url<<endl;
				cout<<data<<endl;
				cout<<t<<","<<team<<","<<val<<endl;
			}
			assert(t==team);
		}else if(n=="location"){
			r.location=ms();
		}else if(n=="key"){
			Team t(s());
			assert(t==team);
		}else if(n=="country_name"){
			r.country=ms();
		}else if(n=="nickname"){
			r.nickname=ms();
		}else if(n=="events"){
			/*for(auto elem:val.get_array()){
				cout<<elem<<endl;
			}
			nyi*/
			//just going to do the stupid thing for now and skip this.
		}else{
			cout<<a<<endl;
			nyi
		}
	}
	return r;//return Team_info{website,name,locality,region,team,country,location,team_number,key,nickname,events};
	#endif
}

vector<string> words(string s){
	//later, should make this more sophisticated.
	return flatten(mapf([](string x){ return split(x,'/'); },split(s)));
}

template<typename T>
bool all_equal(vector<T> v){
	for(auto a:v){
		if(a!=v[0]) return 0;
	}
	return 1;
}

template<typename T>
vector<T> take(unsigned i,set<T> s){
	return take(i,to_vector(s));
}

/*map<unsigned,vector<string>> count2_noncase(vector<string> v){
	
}*/

void scrape_team_demo(){
	auto teams=all_teams();
	/*vector<string> w;
	for(auto team:teams){
		auto m=mapf([team](Year year){ return get(team,year).region; },all_years());
		m=filter([](Maybe<string> m){ return m; },m);
		//cout<<m[0]<<endl;
		if(!all_equal(m)) cout<<count(m)<<endl;
		//assert(all_equal(m));
		if(m.size()){
			//w|=words(*m.at(0));
			w|=*m.at(0);
		}
	}*/
	auto team_info=mapf([](Team t){ return get(t,2014); },teams);
	const auto states=to_set(mapf([](Team_info t){ return t.region; },team_info));
	map<Team,Team_info> t2;
	for(auto a:team_info) t2.insert(make_pair(a.team,a));
	map<Maybe<string>,set<Team>> teams_by_state;
	auto find_state=[=](Team team){
		auto f=t2.find(team);
		assert(f!=end(t2));
		return f->second.region;
	};

	for(auto state:states){
		//cout<<state<<endl;
		auto st_teams=filter(
			[=](Team t){
				return find_state(t)==state;
			},
			teams
		);
		//cout<<st_teams<<endl;
		teams_by_state[state]=st_teams;
	}

	//const auto MATCHES=filter([](Match_info m){ return m.competition_level!=Competition_level::QUALS; },all_matches());
	const auto MATCHES=all_matches();
	
	auto h2h=flat_head_to_head(MATCHES);
	//for each state, print out all the head to head results sorted by common wins column
	auto s=filter(
		[=](Flat_head_to_head f){
			auto t0=get<0>(f);
			auto t1=get<1>(f);
			return t0<t1;
		},
		h2h
	);
	//print_table(reversed(sort_tuples<Flat_head_to_head,5>(s)));
	//this is the good by-state breakdown
	for(auto state:states){
		auto s=filter(
			[=](Flat_head_to_head f){
				auto t0=get<0>(f);
				auto t1=get<1>(f);
				return t0<t1 && find_state(t0)==state && find_state(t1)==state;
			},
			h2h
		);
		//for(auto a:s) cout<<a<<endl;
		if(s.size()){
			cout<<state<<endl;
			print_table(take(10,reversed(sort_tuples<Flat_head_to_head,5>(s))));
		}
	}

	/*cout<<w.size()<<endl;
	w=mapf([](string s){ return toupper(s); },w);
	for(auto a:count2(w)){
		cout<<a<<endl;
	}
	return;
	for(auto year:all_years()){
		for(auto team:teams){
			auto g=get(team,year);
			cout<<g<<endl;
			if(g.nickname){
				w|=words(*g.nickname);
			}
		}
	}*/
}
