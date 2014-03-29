#include<algorithm>
#include<array>
#include "tuple.h"
#include "str.h"
#include "util.h"
#include "set.h"
#include "scrape.h"
#include "map.h"
#include "team.h"
#include "record.h"
#include "default.h"
#include "limited_vector.h"
#include "pick.h"
#include "projected_strength.h"
#include "calculate.h"

using namespace std;

//The blue alliance's 2007 Pittsburgh data is totally bogus - it has team numbers in one of the socre columns for half of the event.
/*
List of integrity checks that should probably run on match data:
-All scores are >=0
-All matches from the same year have the same number of robots on each side?  (At least for official events)
-Match scores don't look like they're supposed to be team numbes.

It would be interesting to attempt to test Karthik's hypothesis that you'd be better off picking a team with high variance if you're one of the lower captains in order to increase your odds of an alliance of better-on paper team.

It would be interesting to try to get the data about what the pick orders was.
*/

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

double rerange(double orig_min,double orig_max,double new_min,double new_max,double value){
	double portion=(value-orig_min)/(orig_max-orig_min);
	return new_min+portion*(new_max-new_min);
}

vector<unsigned> range(unsigned lim){
	vector<unsigned> r;
	for(unsigned i=0;i<lim;i++) r|=i;
	return r;
}

void histogram(ostream& o,vector<int> v){
	static const unsigned WIDTH=80;
	auto mina=min(v),maxa=max(v);
	auto d=mapf([=](int x){ return rerange(mina,maxa,0,80,x); },v);
	for(auto i:d){
		for(auto _:range(i)) o<<"=";
		o<<"\n";
	}
}

//should probably move this to a header someplace
typedef string Event_key;

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

/*map<Team,Record> calculate_records(vector<Match_info> const& m){
	map<Team,Record> r;
	for(Match_info const& match:m){
		auto v=values(match.alliances);
		auto scores=mapf([](Match_info::Alliance a){ return a.score; },v);
		assert(scores.size()==2); //not dealing with more than two alliances at the moment.  
		if(scores[0]==scores[1]){
			auto tie=[&](Team t){ return r[t].tie++; };
			mapf(tie,teams(match));
		}else{
			auto win=[&](Team t){ return r[t].win++; };
			auto loss=[&](Team t){ return r[t].loss++; };
			auto teams=mapf([](Match_info::Alliance a){ return a.teams; },v);
			if(scores[0]>scores[1]){
				mapf(win,teams[0]);
				mapf(loss,teams[1]);
			}else{
				mapf(loss,teams[0]);
				mapf(win,teams[1]);
			}
		}
	}
	return r;
}*/

template<typename K,typename V>
vector<pair<K,V>> to_pairs(map<K,V> m){
	vector<pair<K,V>> r;
	for(auto p:m) r|=p;
	return r;
}

template<class Tuple,int N>
struct Tuple_cmp{
	static bool cmp(Tuple a,Tuple b){
		auto av=get<N>(a);
		auto bv=get<N>(b);
		if(av<bv) return 1;
		if(bv<av) return 0;
		return Tuple_cmp<Tuple,N-1>::cmp(a,b);
	}
};

template<class Tuple>
struct Tuple_cmp<Tuple,0>{
	static bool cmp(Tuple a,Tuple b){
		return (get<0>(a)) < (get<0>(b));
	}
};

template<typename Tuple,int COL>
vector<Tuple> sort_tuples(vector<Tuple> v){
	//eh...this might get pretty annoying to implement pretty fast.
	sort(begin(v),end(v),Tuple_cmp<Tuple,COL>::cmp);
	return v;
}

template<typename Collection>
void print_table(Collection const& c){
	for(auto const& a:c){
		print(cout,"\t",a);
		cout<<"\n";
		//cout<<a<<"\n";
	}
}

template<typename Collection>
void print_html_table(Collection const& c){
	auto title="Head to head records";
	cout<<tag("html",
		tag("head",
			tag("title",title)
		)+
		tag("body",
			tag("table",
				[&](){
					stringstream ss;
					for(auto a:c){
						ss<<tag("tr",[&](){
							stringstream ss;
							ss<<"<td>";
							print(ss,"</td><td>",a);
							ss<<"</td>";
							return ss.str();
						}());
					}
					return ss.str();
				}()
			)
		)
	);
}

vector<Match_info> with_team(vector<Match_info> const& m,Team t){
	return filter([&](Match_info mi){ return teams(mi)&t; },m);
}

vector<Match_info> with_event(vector<Match_info> const& m,Event_key event){
	return filter([&](Match_info mi){ return mi.event==event; },m);
}

//could be called 'team match result'
enum class Match_result{WIN,LOSS,TIE};

ostream& operator<<(ostream& o,Match_result m){
	switch(m){
		#define X(name) case Match_result::name: return o<<""#name;
		X(WIN)
		X(LOSS)
		X(TIE)
		#undef X
		default:
			assert(false);
	}
}

struct Simple_match{
	array<unsigned,2> score;
	array<Limited_vector<Team,3>,2> teams;

	Simple_match(){
		score[0]=score[1]=0;
	}

	Simple_match(Match_info const& m){
		auto v=values(m.alliances);
		assert(v.size()==2);
		for(unsigned i=0;i<2;i++){
			score[i]=v[i].score;
			teams[i]=v[i].teams;
			//cout<<"pre="<<v[i].teams<<"\n";
			//cout<<"post="<<teams[i]<<"\n";
			cout.flush();
			assert(teams[i]==v[i].teams);
		}
	}
};

ostream& operator<<(ostream& o,Simple_match m){
	nyi//return o<<"Simple_match("<<m.score<<" "<<m.teams<<")";
}

vector<Simple_match> simple_matches(vector<Match_info> const& m){
	vector<Simple_match> r;
	for(auto& a:m) r|=Simple_match(a);
	return r;
}

vector<pair<Match_info::Alliance,Match_result>> alliance_result(Match_info const& m){
	vector<pair<Match_info::Alliance,Match_result>> r;
	auto v=values(m.alliances);
	assert(v.size()==2); //not dealing with other cases yet.
	if(v[0].score>v[1].score){
		r|=make_pair(v[0],Match_result::WIN);
		r|=make_pair(v[1],Match_result::LOSS);
	}else if(v[0].score<v[1].score){
		r|=make_pair(v[1],Match_result::WIN);
		r|=make_pair(v[0],Match_result::LOSS);
	}else{
		r|=make_pair(v[0],Match_result::TIE);
		r|=make_pair(v[1],Match_result::TIE);
	}
	return r;
}

vector<pair<Match_info::Alliance,Match_result>> alliance_results(vector<Match_info> const& m){
	return flatten(mapf(alliance_result,m));
}

Record tally(Match_result m){
	switch(m){
		case Match_result::WIN: return Record{1,0,0};
		case Match_result::LOSS: return Record{0,1,0};
		case Match_result::TIE: return Record{0,0,1};
		default:nyi
	}
}

Record tally(vector<Match_result> v){
	Record r;
	for(auto a:v){
		switch(a){
			case Match_result::WIN:
				r.win++;
				break;
			case Match_result::LOSS:
				r.loss++;
				break;
			case Match_result::TIE:
				r.tie++;
				break;
			default:
				assert(false);
		}
	}
	return r;
}

//all of the pairs of members of 's' where the first element is less than the second.
template<typename T>
set<pair<T,T>> pairs(set<T> s){
	set<pair<T,T>> r;
	for(auto a:s){
		for(auto b:s){
			if(a<b){
				r|=make_pair(a,b);
			}
		}
	}
	return r;
}

vector<pair<Team,Team>> mates(Match_info::Alliance a){
	vector<pair<Team,Team>> r;
	nyi
	return r;
}

template<typename T>
map<unsigned,vector<T>> count2(vector<T> v){
	map<unsigned,vector<T>> r;
	for(auto p:count(v)){
		r[p.second]|=p.first;
	}
	return r;
}

template<typename T>
vector<T> reverse(set<T> s){
	vector<T> r1,r;
	for(auto a:s) r1|=a;
	for(int i=r1.size()-1;i>=0;i--){
		r|=r1[i];
	}
	return r;
}

template<typename T,unsigned LEN>
set<T> to_set(array<T,LEN> a){
	set<T> r;
	for(auto elem:a) r|=elem;
	return r;
}

void common_alliances(vector<Match_info> const& m){
	auto c=count2(
		mapf(
			[](Match_info::Alliance m){ return to_set(m.teams); },
			alliances(m)
			/*[](Match_info m){ return teams(m); },
			m*/
		)
	);
	for(auto key:reverse(keys(c))){
		cout<<key<<" "<<c[key].size()<<"\n";
		for(auto a:c[key]){
			cout<<key<<" "<<a<<"\n";
		}
	}
}

enum class Pair_result{TEAM_A,TEAM_B,OPPOSED_TIE,BOTH,NEITHER,TOGETHER_TIE};

ostream& operator<<(ostream& o,Pair_result p){
	#define X(name) if(p==Pair_result::name) return o<<""#name;
	X(TEAM_A)
	X(TEAM_B)
	X(OPPOSED_TIE)
	X(BOTH)
	X(NEITHER)
	X(TOGETHER_TIE)
	#undef X
	nyi
}

static const set<Pair_result> pair_results{
	#define X(name) Pair_result::name
	X(TEAM_A),X(TEAM_B),X(BOTH),X(NEITHER),X(OPPOSED_TIE),X(TOGETHER_TIE)
	#undef X
};

vector<pair<pair<Team,Team>,Pair_result>> pairwise(Simple_match m){
	vector<pair<pair<Team,Team>,Pair_result>> r;
	bool w0=m.score[0]>m.score[1];
	bool w1=m.score[0]<m.score[1];
	auto add=[&](Team a,Team b,Pair_result p){
		//if wanted to, could make it so that didn't have the same team paired with itself here.
		r|=make_pair(make_pair(a,b),p);
	};
	for(auto t1:m.teams[0]){
		for(auto t2:m.teams[0]){
			add(
				t1,t2,
				w0?Pair_result::BOTH:(
					w1?Pair_result::NEITHER:Pair_result::TOGETHER_TIE
				)
			);
		}
		for(auto t2:m.teams[1]){
			add(
				t1,t2,
				w0?Pair_result::TEAM_A:(
					w1?Pair_result::TEAM_B:Pair_result::OPPOSED_TIE
				)
			);
		}
	}
	for(auto t1:m.teams[1]){
		for(auto t2:m.teams[0]){
			add(
				t1,t2,
				w0?Pair_result::TEAM_B:(
					w1?Pair_result::TEAM_A:Pair_result::OPPOSED_TIE
				)
			);
		}
		for(auto t2:m.teams[1]){
			add(
				t1,t2,
				w0?Pair_result::NEITHER:(
					w1?Pair_result::BOTH:Pair_result::TOGETHER_TIE
				)
			);
		}
	}
	return r;
}

//Like the 'group' function in SQL
template<typename Func,typename T>
auto segregate(Func f,vector<T> in)->map<decltype(f(in[0])),vector<T>>{
	map<decltype(f(in[0])),vector<T>> r;
	for(auto a:in){
		r[f(a)]|=a;
	}
	return r;
}

template<typename Func,typename K,typename V>
auto map_map(Func f,map<K,V> m)->map<K,decltype(f(begin(m)->second))>{
	map<K,decltype(f(begin(m)->second))> r;
	for(auto p:m) r[p.first]=f(p.second);
	return r;
}

map<pair<Team,Team>,map<Pair_result,Default<unsigned,0>>> head_to_head(vector<Match_info> const& m){
	auto a=mapf([](Match_info m){ return Simple_match(m); },m);
	auto res=flatten(mapf(
		[](Simple_match s){ return pairwise(s); },
		a
	));
	auto seg=segregate([](pair<pair<Team,Team>,Pair_result> p){ return p.first; },res);
	
	return map_map(
		[](vector<pair<pair<Team,Team>,Pair_result>> vp){
			return count(mapf(
				[](pair<pair<Team,Team>,Pair_result> p){ return p.second; },
				vp
			));
		},
		seg
	);
}

typedef unsigned U;
typedef tuple<Team,Team,U,U,U,U,U,U> Flat_head_to_head;
vector<Flat_head_to_head> flat_head_to_head(vector<Match_info> const& m){
	vector<Flat_head_to_head> r;
	for(auto a:head_to_head(m)){
		auto c=[&](Pair_result p){
			return a.second[p];
		};
		Flat_head_to_head t2=make_tuple(
			a.first.first,
			a.first.second,
			c(Pair_result::TEAM_A),
			c(Pair_result::TEAM_B),
			c(Pair_result::OPPOSED_TIE),
			c(Pair_result::BOTH),
			c(Pair_result::NEITHER),
			c(Pair_result::TOGETHER_TIE)
		);
		r|=t2;
	}
	return r;
}

//Returns the record of the teams together and opposing each other.  
map<pair<Team,Team>,pair<Record,Record>> head_to_head1(vector<Match_info> const& m){
	map<pair<Team,Team>,pair<Record,Record>> r;
	for(auto team:teams(m)){
		/*auto m2=filter(
			[&](Match_info mi){
				return contains(teams(mi),team);
			},
			m
		);*/
		auto m2=with_team(m,team);
		auto second_teams=filter([team](Team t){ return t>team; },teams(m2));
		for(auto t2:second_teams){
			auto p=make_pair(team,t2);
			//cout<<p<<"\n";
			auto m3=with_team(m2,t2);
			auto ar=alliance_results(m3);//bug:this should be m3
			//cout<<ar<<"\n";
			Record r1,r2,both;
			for(auto const& a:ar){
				auto alliance_teams=to_set(a.first.teams);
				auto has_t1=contains(alliance_teams,team);
				auto has_t2=contains(alliance_teams,t2);
				auto rec=tally(a.second);
				if(has_t1 && has_t2){
					both+=rec;
				}else if(has_t1){
					r1+=rec;
				}else if(has_t2){
					r2+=rec;
				}
			}
			assert(r1==reversed(r2));
			assert(sum(r1)+sum(both)==m3.size());
			//cout<<"r1="<<
			r[p]=make_pair(both,r1);
		}
	}
	return r;
}

template<typename T>
vector<T> reversed(vector<T> v){
	vector<T> r;
	for(int i=v.size()-1;i>=0;i--){
		r|=v[i];
	}
	return r;
}

string to_event_key(string s){
	return string("2013")+tolower(s);
}

template<typename T>
pair<set<T>,set<T>> uniques(set<T> a,set<T> b){
	return make_pair(a_and_not_b(a,b),a_and_not_b(b,a));
}

//returns 0-based number
unsigned alliance_number(unsigned qf_number /*one-based*/,string side_name){
	bool red;
	if(side_name=="red") red=1;
	else if(side_name=="blue") red=0;
	else nyi

	switch(qf_number){
		case 1: return red?0:7;
		case 2: return red?3:4;
		case 3: return red?1:6;
		case 4: return red?2:5;
		default:
			assert(false);
	}
}

map<Event_key,array<set<Team>,8>> finals_alliances(vector<Match_info> m){
	map<Event_key,array<set<Team>,8>> r;
	for(auto match:m){
		if(match.competition_level!=Competition_level::QUARTERS) continue;
		if(match.match_number!=1){
			continue;//(maybe a backup won't be called yet?)
		}
		//cout<<"key="<<match.key<<"\n";
		//cout<<match.match_number<<" "<<match.set_number<<" "<<match.key<<"\n";
		for(auto p:match.alliances){
			auto side=p.first;
			auto team_set=to_set(p.second.teams);
			auto n=alliance_number(match.set_number,side);
			set<Team> &teams=r[match.event][n];
			if(teams.size()){
				if(teams!=team_set){
					cout<<"Teams="<<teams<<"\n";
					cout<<"temss="<<team_set<<"\n";
				}
				assert(teams==team_set);
			}else{
				teams=team_set;
			}
		}
	}
	return r;
}

vector<Match_info> eliminations(vector<Match_info> m){
	return filter([](Match_info m){ return m.competition_level!=Competition_level::QUALS; },m);
}

vector<Match_info> finals(vector<Match_info> m){
	return filter([](Match_info m){ return m.competition_level==Competition_level::FINALS; },m);
}

vector<Match_info> quals(vector<Match_info> m){
	return filter([](Match_info m){ return m.competition_level==Competition_level::QUALS; },m);
}

struct Elim_matchup{
	vector<Match_info> v; //where event, etc, are all the same.
};

struct Elimination_competition_level{
	//if wanted to, could actually make this check that it isn't QUALS.
	Competition_level data;

	operator Competition_level()const{ return data; }
};

//might be useful to have a version of this that didn't require them to be elimination matches.
struct Elim_set{
	string event_code;
	Elimination_competition_level level;
	int set_number;
};

bool operator==(Elim_set a,Elim_set b){
	return a.event_code==b.event_code && a.level==b.level && a.set_number==b.set_number;
}

bool operator<(Elim_set a,Elim_set b){
	return make_tuple(a.event_code,a.level,a.set_number)<make_tuple(b.event_code,b.level,b.set_number);
}

ostream& operator<<(ostream& o,Elim_set e){
	return o<<"Elim_set("<<e.event_code<<" "<<e.level<<" "<<e.set_number<<")";
}

Maybe<Elim_set> elim_set(Match_info m){
	if(m.competition_level==Competition_level::QUALS) return Maybe<Elim_set>();
	return Elim_set{m.event,Elimination_competition_level{m.competition_level},m.set_number};
}

map<Event_key,array<vector<set<Team>>,8>> finals_alliance_combinations(vector<Match_info> m){
	//for(auto a:to_set(mapf(elim_set,m))) cout<<"== "<<a<<"\n";
	auto grouped=segregate(elim_set,m);
	//for(auto a:grouped) cout<<a<<"\n";
	auto mm=mapf(
		[](pair<Maybe<Elim_set>,vector<Match_info>> p){ return p.second.size(); },
		grouped
	);
	cout<<"Number of times that a series was certain # of games:"<<count(mm)<<"\n";
	auto x=to_set(mapf(elim_set,m));
	cout<<x<<"\n";
	nyi

	map<Event_key, array<vector<set<Team>>,8> > r;
	for(auto match:eliminations(m)){
		for(auto p:match.alliances){
			auto side=p.first;
			auto team_set=to_set(p.second.teams);
			//TODO: Fix this logic about what alliance # it is.  This function only works for the quarter-finals
			auto n=alliance_number(match.set_number,side);
			r[match.event][n]|=team_set;
		}
	}
	return r;
}

template<typename T>
set<T> flatten(vector<set<T>> v){
	set<T> r;
	for(auto a:v) r|=a;
	return r;
}

template<typename T>
void chart_membership(vector<set<T>> v){
	cout<<"\t"<<flatten(v)<<"\n";
	//nyi
}

void correlate_picks(){
	auto f=finals_alliance_combinations(matches(2013));
	//cout<<f<<"\n";
	for(auto p:f){
		cout<<p.first<<"\n";
		for(auto a:p.second){
			chart_membership(a);
		}
	}

	auto p1=parse_picks();
	map<Event_key,Event_picks> p;
	for(auto a:p1){
		p.insert(make_pair(to_event_key(a.first),a.second));
	}

	auto g=to_set(get_event_keys(2013));
	//auto m=matches(2013); //this is the only year I have pick data for
	auto events=keys(p);
	cout<<events<<"\n";
	cout<<g<<"\n";
	cout<<"sym:"<<symmetric_difference(events,g)<<"\n";
	cout<<uniques(events,g)<<"\n";
	//map<string,pair<Event_picks,
	auto alliances_by_matches=finals_alliances(matches(2013));
	//for(auto a:f) cout<<a<<"\n";
	//cout<<"hree\n";
	//for(auto p1:p) cout<<p1<<"\n";
	//cout<<"gg\n";
	for(auto event:events){
		cout<<event<<"\n";
		auto f=p.find(event);
		if(f==p.end()){
			cout<<"No data to correlate\n";
			continue;
		}
		assert(f!=p.end());
		Event_picks event_picks=f->second;
		for(unsigned i=0;i<8;i++){
			auto a=to_set(event_picks[i].teams);
			auto b=alliances_by_matches[event][i];
			if(a!=b) cout<<i<<": "<<a<<" "<<b<<"\n";
			//assert( a==b );
		}
	}
}

map<Team,pair<Default<unsigned,0>,Default<unsigned,0>>> finals_appearances(vector<Match_info> m){
	map<Team,pair<Default<unsigned,0>,Default<unsigned,0>>> r;
	for(auto event_p:segregate([](Match_info m){ return m.event; },m)){
		auto event_matches=event_p.second;
		for(auto team:teams(event_matches)){
			auto w=with_team(event_matches,team);
			assert(w.size());
			auto e=eliminations(w);
			r[team].first++;
			if(e.size()) r[team].second++;
		}
	}
	return r;
}

template<typename T>
T last(vector<T> v){
	assert(v.size());
	return v[v.size()-1];
}

//maybe this should be moved to match_info.cpp
set<Team> teams(Match_info::Alliance a){
	return to_set(a.teams);
}

set<Team> winners(Match_info m){
	auto w=winning_alliances(vector<Match_info>{m});
	assert(w.size()==1);//won't happen if we've got a tie
	return teams(w[0]);
}

int run_main(map<string,vector<string>> const& flags){
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

	if(flags.find("project")!=flags.end()){
		return project(flags);
	}

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
		cout<<"quartiles:"<<quartiles(scores(m))<<"\n";
		cout<<"Mean given won="<<mean_score(winning_alliances(m))<<"\n";
		cout<<"Mean given lost="<<mean_score(losing_alliances(m))<<"\n";
	}
	if(get_flag("records")){
		auto c=calculate_records(m);
		auto rec=mapf([](pair<Team,Record> p){ return make_tuple(p.first,p.second,win_portion(p.second)); },c);
		rec=sort_tuples<tuple<Team,Record,double>,2>(rec);
		//cout<<rec<<"\n";
		for(auto a:rec){
			TuplePrinter<tuple<Team,Record,double>,3>::print(cout,"\t",a);
			cout<<"\n";
		}
		//print_table(head_to_head(m));
		/*for(auto a:c){
			//cout<<make_tuple(a.first,a.second,win_portion(a.second))<<"\n";
			cout<<a.first<<"\t"<<a.second<<"\t"<<win_portion(a.second)<<"\n";
		}*/
	}
	if(get_flag("head_to_head")){
		typedef tuple<Team,Team,unsigned,unsigned,unsigned,unsigned> TT;
		/*print_table(sort_tuples<TT,3>(mapf(
			[](pair<pair<Team,Team>,pair<Record,Record>> p){
				return make_tuple(p.first.first,p.first.second,p.second.first,p.second.second);
			},
			head_to_head(m)
		)));*/
		print_table(reversed(sort_tuples<Flat_head_to_head,2>(flat_head_to_head(m))));
	}
	if(get_flag("html_head_to_head")){
		//Warning: This will create a file whole size is measured in megabytes.
		//The problem with that is that you may crash Firefox or Konqueror, lynx appears to hang, etc.  elinks seems to be fine though.  
		//You probably just want this output as text.
		print_html_table(reversed(sort_tuples<Flat_head_to_head,2>(flat_head_to_head(m))));
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
	if(get_flag("correlate_picks")){
		correlate_picks();
	}
	if(get_flag("finals")){
		auto f=finals_appearances(m);
		typedef tuple<Team,unsigned,unsigned,unsigned> Tup;
		vector<Tup> v;
		for(auto p:f){
			auto team=p.first;
			auto events=p.second.first;
			auto elims=p.second.second;
			v|=Tup(team,events,elims,events-elims);
		}
		v=sort_tuples<Tup,3>(v);
		//v=filter([](Tup t){ return get<2>(t)==0; },v);
		print_table(v);
		//for(auto a:v) cout<<a<<"\n";
	}
	if(get_flag("rank")){
		//try to figure out which alliance has won an event with the worst record for its members before the elimination rounds.
		typedef tuple<Event_key,set<Team>,double> Tup;
		vector<Tup> v;
		vector<Event_key> no_finals;
		for(auto event_p:segregate([](Match_info mi){ return mi.event; },m)){
			auto event_key=event_p.first;
			auto matches=event_p.second;
			auto finals_matches=finals(matches);
			if(finals_matches.size()==0){
				no_finals|=event_key;
				continue;
			}
			auto winning_teams=winners(last(finals_matches));
			auto qual_matches=quals(matches);
			auto recs=calculate_records(qual_matches);
			//need to figure out the winning alliance
			//then, for each of its members, find their qualification record
			//add them all up and put into the vector.

			//to start with, going to start by just figuring out the record of everyone in the finals.
			//auto winning_teams=teams(finals_matches);
			Record winning_alliance_record;
			for(auto team:winning_teams){
				winning_alliance_record+=recs[team];
			}
			//cout<<event_p.first<<" "<<winning_alliance_record<<" "<<win_portion(winning_alliance_record)<<"\n";
			v|=make_tuple(event_p.first,winning_teams,win_portion(winning_alliance_record));
		}
		v=sort_tuples<Tup,2>(v);
		print_table(v);
		cout<<"No finals results:"<<no_finals<<"\n";
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
