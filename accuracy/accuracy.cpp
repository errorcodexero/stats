#include<iostream>
#include<fstream>
#include "util.h"
#include "team.h"
#include "str.h"
#include "map.h"

using namespace std;

typedef string Prediction;

map<Team,Prediction> read_prediction_file(string const& filename){
	map<Team,Prediction> r;
	for(auto a:skip(4,lines(slurp(filename)))){
		auto sp=split(a,'\t');
		//cout<<sp<<endl;
		assert(sp.size()==3);
		auto rank=sp[0];
		auto prediction=sp[1];
		auto team=Team(sp[2]);
		r[team]=prediction;
	}
	return r;
}

struct Entry{
	string rank,district_points;
	Team team;
	string event1,event2;
	bool dcmp;
	string frc_cmp,rookie_bonus;
};

ostream& operator<<(ostream& o,Entry)nyi

vector<Entry> read_district_rankings(string const& filename){
	vector<Entry> r;
	for(auto line:lines(slurp(filename))){
		auto sp=split(line,'\t');
		//cout<<sp<<endl;
		assert(sp.size()==8);
		auto rank=sp.at(0);
		auto district_points=sp.at(1);
		auto team=Team{sp.at(2)};
		auto event1=sp.at(3);
		auto event2=sp.at(4);
		bool dcmp;
		{
			auto x=strip(sp.at(5));
			if(x=="---"){
				dcmp=0;
			}else if(x=="Qualified"){
				dcmp=1;
			}else{
				cout<<"x=\""<<x<<"\""<<endl;
				assert(0);
			}
		}
		auto frc_cmp=sp.at(6);
		auto rookie_bonus=sp.at(7);
		r|=Entry{rank,district_points,team,event1,event2,dcmp,frc_cmp,rookie_bonus};
	}
	return r;
}

map<Team,bool> is_qualified(string const& filename){
	auto data=read_district_rankings(filename);
	map<Team,bool> r;
	for(auto a:data){
		r[a.team]=a.dcmp;
	}
	return r;
}

#ifdef ACC
void main1(string infile){
	auto a=read_prediction_file(infile);
//	cout<<a<<endl;
//	cout<<values(a)<<endl;
//	cout<<count(values(a))<<endl;

	auto b=is_qualified("actual.txt");
	//cout<<b<<endl;

	assert(keys(a)==keys(b));

	map<Prediction,vector<bool>> v;
	for(auto team:keys(b)){
		//cout<<team<<"\t";
		//cout<<a[team]<<"\t";
		//cout<<b[team]<<endl;
		v[a[team]].push_back(b[team]);
	}
	//cout<<v<<endl;
	cout<<"Predicted\tActual In\tActual Out\n";
	for(auto a:v){
		auto c=count(a.second);
		cout<<a.first<<"\t\t"<<c[1]<<"\t\t"<<c[0]<<endl;
	}
}

int main(){
	cout<<"March 26 Predictions:\n";
	main1("result1.txt");
	cout<<"March 31 Predictions:\n";
	main1("result2.txt");
}
#endif
