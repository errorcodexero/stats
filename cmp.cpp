#include<vector>
#include<iostream>
#include "team.h"
#include "maybe.h"
#include "util.h"
#include "str.h"

using namespace std;

struct Event_appearance{
	string event_name;
	int pts;
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
	Maybe<int> event1_pts,event2_pts;//todo: Make these of type Event_appearance
	Maybe<int> district_cmp_pts;
	int rookie_bonus;

	//Team_info():rank(-1),total_pts(-1),rookie_bonus(-1){}

	bool valid()const{
		return rank>0 && total_pts>=0 && (rookie_bonus==5 || rookie_bonus==10 || rookie_bonus==0);
	}
};

ostream& operator<<(ostream& o,Team_info a){
	o<<"Team_info( ";
	#define X(name) o<<""#name":"<<a.name<<" ";
	X(rank)
	X(total_pts)
	X(number)
	X(nickname)
	X(event1_pts)
	X(event2_pts)
	X(district_cmp_pts)
	X(rookie_bonus)
	#undef X
	return o<<")";
}

int atoi(string s){
	return ::atoi(s.c_str());
}

Maybe<Team_info> parse_line(string s){
	if(s.size()==0) return Maybe<Team_info>();
	auto v=split(s,'\t');
	auto team_sp=split(v.at(2),'-');
	assert(team_sp.size()>=2);
	Team team(team_sp[0]);
	auto nickname=strip(join(tail(team_sp),'-'));
	Maybe<int> event1=atoi(v.at(3)),event2=atoi(v.at(4)),dcmp=0;
	int rookie_bonus=atoi(v.at(6));
	return Team_info{atoi(v.at(0)),atoi(v.at(1)),team,nickname,event1,event2,dcmp,rookie_bonus};
}

vector<Team_info> read_data(){
	vector<Team_info> r;
	for(auto a:lines(slurp("dcmp_data.txt"))){
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

#ifdef CMP
int main(){
	cout<<"hello\n";
	auto d=read_data();
	auto t=total_pts(d);
	cout<<mean(t)<<"\n";
	cout<<sum(t)<<"\n";
	cout<<quartiles(t)<<"\n";
	return 0;
}
#endif
