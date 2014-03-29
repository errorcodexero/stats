#include "pick.h"
#include<cassert>
#include "dirent.h"
#include "util.h"
#include "str.h"
#include "default.h"
#include "map.h"

using namespace std;

ostream& operator<<(ostream& o,Alliance_result a){
	#define X(name) if(a==Alliance_result::name) return o<<""#name;
	X(W)
	X(F)
	X(SF)
	X(QF)
	#undef X
	nyi
}

ostream& operator<<(ostream& o,Alliance a){
	//o<<"Alliance("<<a.teams<<" "<<a.result<<")";
	nyi
}

//eventually, should move to util.
vector<string> getdir(string dir)
{
	vector<string> files;
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        stringstream ss;
		ss << "Error(" << errno << ") opening " << dir << endl;
        throw ss.str();
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return files;
}

Alliance_result parse_alliance_result(string s){
	#define X(name) if(s==""#name) return Alliance_result::name;
	X(W)
	X(F)
	X(SF)
	X(QF)
	#undef X
	nyi
}

Alliance parse_line(string s){
	auto sp=split(s,',');
	assert(sp.size()==5);
	//cout<<sp<<"\n";
	return Alliance{{Team(sp[1]),Team(sp[2]),Team(sp[3])},parse_alliance_result(sp[4])};
}

Event_picks parse_event(string s){
	auto l=lines(s);
	//cout<<l<<"\n";
	//cout<<l.size()<<"\n";
	assert(l.size()==10);
	vector<Alliance> a;
	for(unsigned i=1;i<9;i++){
		a|=parse_line(l[i]);
	}
	assert(a.size()==8);

	//there's got to be a better way to write this line.
	return Event_picks{a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]};
}

bool suffix(string haystack,string needle){
	if(haystack.size()<needle.size()) return 0;
	return haystack.substr(haystack.size()-needle.size(),needle.size())==needle;
}

map<string,Event_picks> parse_picks(){
	string dir="2013_pick_data";
	auto f=filter([](string s){ return prefix(s,"event_"); },getdir(dir));
	//cout<<f<<"\n";
	map<string,Event_picks> r;
	for(auto s:f){
		assert(prefix(s,"event_"));
		assert(suffix(s,".csv"));
		auto event_code=s.substr(6,s.size()-6-4);
		//cout<<"event_code="<<event_code<<"\n";
		auto filename=dir+"/"+s;
		r.insert(make_pair(event_code,parse_event(slurp(filename))));
	}
	return r;
}

int pick_main(){
	auto picks=parse_picks();
	//cout<<parse_picks()<<"\n";
	/*
	Count how many times each team ended w/ a given result.
	map<Team,vector<Alliance_result>> m;
	for(auto p:picks){
		auto data=p.second;
		for(auto alliance:data.
	}
	cout<<m;
	*/

	//count how many times each seed got what result
	map<int,map<Alliance_result,Default<unsigned,0>>> m;
	for(auto p:picks){
		for(unsigned i=0;i<8;i++){
			m[i][p.second[i].result]++;
		}
	}
	//cout<<m<<"\n";
	for(auto a:m){
		cout<<a<<"\n";
	}

	cout<<"# of times that each seed got a result at least as good as X\n";
	array<array<unsigned,4>,8> aa;
	for(unsigned i=0;i<8;i++){
		aa[i][0]=m[i][Alliance_result::W];
		aa[i][1]=m[i][Alliance_result::F]+aa[i][0];
		aa[i][2]=m[i][Alliance_result::SF]+aa[i][1];
		aa[i][3]=m[i][Alliance_result::QF]+aa[i][2];
	}
	for(auto a:aa){
		nyi//cout<<a<<"\n";
	}

	cout<<"winning \% at each round, for each seed\n";
	array<array<unsigned,3>,8> seed_pcnt;
	for(unsigned i=0;i<8;i++){
		for(unsigned j=0;j<3;j++){
			seed_pcnt[i][j]=100*((double)aa[i][j])/aa[i][j+1];
		}
	}
	for(auto a:seed_pcnt){
		nyi//cout<<a<<"\n";
	}

	//a more complicated thing to try would be to figure out what the winning % between each seed is.  
	//bracket looks like this: ((18)(45))((36)(27))
	auto matchups=[](Event_picks p){
		vector<pair<int,int>> r;
		//unsigned t18=p[0].result<p[7].result;
		//r[0]=(p[0].result<p[7].result)?make_pair(0,7):make_pair(7,0);

		//nyi
		auto elim=[&](int i1,int i2){
			bool a=p[i1].result<p[i2].result;
			r|=(a?make_pair(i1,i2):make_pair(i2,i1));
			return a?i1:i2;
		};
		vector<int> quarters={elim(0,7),elim(3,4),elim(1,6),elim(2,5)};
		vector<int> semis={elim(quarters[0],quarters[1]),elim(quarters[2],quarters[3])};
		elim(semis[0],semis[1]);
		return r;
	};

	Event_picks some_pick=begin(picks)->second;
	cout<<matchups(some_pick)<<"\n";

	auto f=flatten(mapf(matchups,values(picks)));
	auto c=count(f);
	for(auto a:c){
		auto t=a.first;
		auto won=a.second;
		auto lost=c[make_pair(t.second,t.first)];
		cout<<t.first+1<<" "<<t.second+1<<"\t"<<won<<"-"<<lost<<"\t"<<((double)won/(won+lost))<<"\n";
	}
}
