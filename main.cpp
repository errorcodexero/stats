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

map<Team,Record> calculate_records(vector<Match_info> const& m){
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
}

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

template<typename T,unsigned N>
ostream& operator<<(ostream& o,array<T,N> a){
	o<<"[ ";
	for(auto elem:a) o<<elem<<" ";
	return o<<"]";
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
	return o<<"Simple_match("<<m.score<<" "<<m.teams<<")";
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

//could also do a multiset...
template<typename T>
map<T,Default<unsigned,0>> count(vector<T> v){
	map<T,Default<unsigned,0>> r;
	for(auto a:v) r[a]++;
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

set<pair<pair<Team,Team>,Pair_result>> pairwise(Simple_match m){
	set<pair<pair<Team,Team>,Pair_result>> r;
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

template<typename T>
vector<T> to_vector(set<T> s){
	vector<T> r;
	for(auto a:s) r|=a;
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

//might want to make return Default<unsigned,0> instead of unsigned.
map<pair<Team,Team>,map<Pair_result,Default<unsigned,0>>> head_to_head(vector<Match_info> const& m){
	auto a=mapf([](Match_info m){ return Simple_match(m); },m);
	auto res=flatten(mapf(
		[](Simple_match s){ return to_vector(pairwise(s)); },
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

typedef tuple<Team,Team,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned> Flat_head_to_head;
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
