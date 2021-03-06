#include<vector>
#include<iostream>
#include<chrono>
#include "team.h"
#include "maybe.h"
#include "util.h"
#include "str.h"
#include "set.h"

using namespace std;

typedef string Event;

struct Event_appearance{
	int pts;
	Event event_name;
};

ostream& operator<<(ostream& o,Event_appearance a){
	o<<"Event_appearance("<<a.event_name<<" "<<a.pts<<")";
	return o;
}

struct Team_info{
	int rank;
	int total_pts;
	Team number;
	string nickname;
	Maybe<Event_appearance> event1,event2;

	enum class District_cmp_status{QUALIFIED,DECLINED,NOT_QUALIFIED};

	District_cmp_status district_cmp_status;
	Maybe<int> district_cmp_pts;
	int rookie_bonus;

	Team_info(int a,int b,Team c,string d,Maybe<Event_appearance> e,Maybe<Event_appearance> f,District_cmp_status g,Maybe<int> h,int i):
		rank(a),total_pts(b),number(c),nickname(d),event1(e),event2(f),district_cmp_status(g),district_cmp_pts(h),rookie_bonus(i)
	{
		assert(valid());
	}

	bool valid()const{
		if(event2 && !event1) return 0;
		return rank>0 && total_pts>=0 && (rookie_bonus==5 || rookie_bonus==10 || rookie_bonus==0);
	}
};

Maybe<Team_info::District_cmp_status> parse_district_cmp_status(string s){
	s=strip(s);
	if(s=="DI") return Team_info::District_cmp_status::DECLINED;
	if(s=="Qualified") return Team_info::District_cmp_status::QUALIFIED;
	if(s=="---") return Team_info::District_cmp_status::NOT_QUALIFIED;
	return Maybe<Team_info::District_cmp_status>();
}

ostream& operator<<(ostream& o,Team_info::District_cmp_status a){
	#define X(name) if(a==Team_info::District_cmp_status::name) return o<<""#name;
	X(QUALIFIED)
	X(DECLINED)
	X(NOT_QUALIFIED)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Team_info a){
	o<<"Team_info( ";
	#define X(name) o<<""#name":"<<a.name<<" ";
	X(rank)
	X(total_pts)
	X(number)
	X(nickname)
	X(event1)
	X(event2)
	X(district_cmp_status)
	X(district_cmp_pts)
	X(rookie_bonus)
	#undef X
	return o<<")";
}

Maybe<Event_appearance> appearance(string s){
	//cout<<"\""<<s<<"\""<<endl;
	if(s=="Not Played ") return Maybe<Event_appearance>();
	auto v=split(s,'[');
	return Event_appearance{atoi(v.at(0)),v.at(1).substr(0,v.at(1).size()-2)};
}

int parse_int(string s){
	int r=atoi(s.c_str());
	assert(as_string(r)==s);
	return r;
}

static Maybe<Team_info> parse_line(string s){
	if(s.size()==0) return Maybe<Team_info>();
	auto v=split(s,'\t');
	auto team_sp=split(v.at(2),'-');
	assert(team_sp.size()>=2);
	Team team(team_sp[0]);
	auto nickname=strip(join(tail(team_sp),'-'));
	Maybe<Event_appearance> event1=appearance(v.at(3)),event2=appearance(v.at(4));
	int rookie_bonus=parse_int(v.at(7));
	return Team_info{
		atoi(v.at(0)),//rank
		atoi(v.at(1)),//total pts
		team,//number
		nickname,event1,event2,
		*parse_district_cmp_status(v.at(5)),//district cmp status
		atoi(v.at(6)),//district cmp points
		rookie_bonus
	};
}

vector<Team_info> read_data(){
	vector<Team_info> r;
	//for(auto a:lines(slurp("dcmp_data.txt"))){
	for(auto a:lines(slurp("page_apr8.txt"))){
		auto p=parse_line(a);
		if(p) r|=*p;
	}
	return r;
}

vector<int> total_pts(vector<Team_info> v){
	vector<int> r;
	for(auto a:v) r|=a.total_pts;
	return r;
}

vector<Event> events1(Team_info t){
	vector<Event> r;
	if(t.event1) r|=t.event1->event_name;
	if(t.event2) r|=t.event2->event_name;
	return r;
}

template<typename T>
set<T> or_all(vector<set<T>> v){
	set<T> r;
	for(auto a:v) r|=a;
	return r;
}

vector<Event> events(vector<Team_info> v){
	return flatten(mapf(events1,v));
}

vector<Event_appearance> event_appearances1(Team_info t){
	vector<Event_appearance> v;
	if(t.event1) v|=*t.event1;
	if(t.event2) v|=*t.event2;
	return v;
}

vector<Event_appearance> event_appearances(vector<Team_info> v){
	return flatten(mapf(event_appearances1,v));
}

vector<int> points(vector<Event_appearance> v){
	return mapf([](Event_appearance e){ return e.pts; },v);
}

map<Event,vector<int>> point_totals_by_event(vector<Team_info> v){
	auto e=event_appearances(v);
	return map_map(
		points,
		segregate([](Event_appearance e){ return e.event_name; },e)
	);
}

map<Event,int> points_by_event(vector<Team_info> v){
	return map_map(
		[](vector<int> v){
			return sum(v);
		},
		point_totals_by_event(v)
	);
}

map<Event,Maybe<double>> points_per_team_by_event(vector<Team_info> v){
	return map_map(
		[](vector<int> v){ return mean(v); },
		point_totals_by_event(v)
	);
}

template<typename A,typename B>
vector<pair<A,B>> to_vector(map<A,B> m){
	vector<pair<A,B>> r;
	for(auto p:m) r|=make_pair(p.first,p.second);
	return r;
}

template<typename K,typename V>
void print_map(map<K,V> m){
	for(auto a:sorted(reverse_pairs(to_vector(m)))){
		cout<<a<<endl;//cout<<a.second<<"\t"<<a.first<<endl;
	}
}

vector<pair<int,int>> perf_pairs(vector<Team_info> v){
	vector<pair<int,int>> r;
	for(auto a:v){
		if(a.event1 && a.event2){
			auto e1=a.event1->pts,e2=a.event2->pts;
			r|=make_pair(e1,e2);
			r|=make_pair(e2,e1);
		}
	}
	return r;
}

set<int> totals(vector<Team_info> d){
	return to_set(points(event_appearances(d)));
}

pair<int,int> seen_range(vector<Team_info> v,int pts){
	auto p=perf_pairs(v);
	auto lower=min(mapf(
		[](pair<int,int> p){ return p.second; },
		filter([pts](pair<int,int> p){ return p.first>=pts; },p)
	));
	auto upper=max(mapf(
		[](pair<int,int> p){ return p.second; },
		filter([pts](pair<int,int> p){ return p.first<=pts; },p)
	));
	return make_pair(lower,upper);
}

vector<Team_info> whole_events_only(vector<Team_info> v){
	auto m=points_per_team_by_event(v);
	set<Event> completed_events=to_set(
		mapf(
			[](pair<const Event,Maybe<double>> p){ return p.first; },
			filter(
				[](pair<const Event,Maybe<double>> p){ return p.second && *p.second>20; },
				to_vector(m)
			)
		)
	);
	cout<<"done:"<<completed_events<<"\n";
	for(auto& elem:v){
		auto clear_if_incomplete=[=](Maybe<Event_appearance> &m){
			if(!m) return;
			if( !(m->event_name&completed_events) ){
				m=Maybe<Event_appearance>();
			}
		};
		clear_if_incomplete(elem.event1);
		clear_if_incomplete(elem.event2);
	}
	return v;
}

/*vector<Team_info> fill_in_high(vector<Team_info> v){
	
}*/

static const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
static auto rng=std::default_random_engine(seed);

template<typename T>
T shuffle(T t){
	shuffle (begin(t),end(t),rng);
	return t;
}

vector<Team> choose_for_match(vector<Team> teams){
	shuffle(teams);
	return take(6,teams);
}

template<typename T>
vector<vector<T>> triples(vector<T> in){
	assert(in.size()%3==0);
	vector<vector<T>> r;
	for(unsigned i=0;i<in.size();i+=3){
		vector<T> v;
		for(unsigned j=0;j<3;j++){
			v|=in[i+j];
		}
		r|=v;
	}
	return r;
}

template<typename T>
T choose_random(vector<T> v){
	return v[rand()%v.size()];
}

map<Team,unsigned> dcmp_dist_random(vector<Team> teams){
	static const unsigned TEAMS=64;
	assert(TEAMS==teams.size());

	map<Team,unsigned> pts;
	auto add=[&](Team team,unsigned p){
		auto f=pts.find(team);
		if(f==end(pts)) pts[team]=0;
		pts[team]+=p;
	};

	//qualifications
	static const unsigned MATCHES=70;
	for(unsigned i=0;i<MATCHES;i++){
		auto in=choose_for_match(teams);
		for(unsigned i=0;i<3;i++){
			add(in[i],2);
		}
	}

	//awards
	for(auto a:enumerate(take(14,shuffle(teams)))){
		int value=(a.first==0)?10:((a.first==1)?8:5);
		add(a.second,value);
	}

	//should make the elim teams be the top16 in pts after prelims & the rest random.
	auto elim_teams=take(24,shuffle(teams));
	cout<<elim_teams<<endl;
	auto alliances=triples(elim_teams);
	//alliance selection points
	for(unsigned i=0;i<8;i++){
		for(unsigned j=0;j<2;j++){
			add(alliances[i][j],16-i);
		}
		add(alliances[i][2],1+i);
	}

	//elimination matches
	//quarter finals
	vector<vector<Team>> semi_finalists;
	for(unsigned i=0;i<8;i+=2){
		semi_finalists|=alliances[i+(rand()%2)];
	}
	for(Team team:flatten(semi_finalists)) add(team,10);

	vector<vector<Team>> finalists;
	for(unsigned i=0;i<4;i++){
		finalists|=semi_finalists[i+(rand()%2)];
	}
	for(auto team:flatten(finalists)) add(team,10);

	//winners
	for(auto team:choose_random(finalists)) add(team,10);

	return pts;
}

//the top 64; for now we'll just do the number of points accumulated.
vector<Team> top_teams(vector<Team_info> v){
	assert(v.size()>=64);
	vector<Team> r;
	for(unsigned i=0;i<64;i++){
		r|=v[i].number;
	}
	return r;
}

#ifdef CMP
int main(){
	cout<<"hello\n";
	auto d=read_data();
	for(auto a:d) cout<<a<<endl;
	d=whole_events_only(d);
	cout<<count(events(d))<<endl;
	print_map(points_by_event(d));
	//for(auto a:sorted(reverse_pairs(to_vector(points_by_event(d))))){
	//	cout<<a<<endl;//cout<<a.second<<"\t"<<a.first<<endl;
	//}
	//for(auto a:d)cout<<a<<endl;
	print_map(points_per_team_by_event(d));
	auto t=total_pts(d);
	cout<<mean(t)<<"\n";
	cout<<sum(t)<<"\n";
	cout<<quartiles(t)<<"\n";
	//cout<<perf_pairs(d)<<endl;
	/*for(auto t:totals(d)){
		cout<<t<<"\t"<<seen_range(d,t)<<endl;
	}*/
	cout<<dcmp_dist_random(top_teams(d))<<endl;

	auto team_number=[](Team_info t){ return t.number; };
	auto s=segregate([](Team_info t){ return t.district_cmp_status; },d);
	auto m=map_map([=](vector<Team_info> p){ return mapf(team_number,p); },s);
	cout<<m<<endl;
	for(auto p:m){
		cout<<p.first<<" "<<p.second.size()<<endl<<"\t"<<sorted(p.second)<<endl;
	}
	return 0;
}
#endif
