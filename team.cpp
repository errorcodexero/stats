#include "team.h"
#include<iostream>
#include<cassert>
#include "util.h"

using namespace std;

Team::Team(string a):b(0){
	if(prefix(a,"frc")) a=a.substr(3,a.size()-3);
	assert(a.size());
	char c=a[a.size()-1];
	if(c=='b'){
		b=1;
		a=a.substr(0,a.size()-1);
	}
	data=atoi(a.c_str());
	assert(data);
}

ostream& operator<<(ostream& o,Team const& t){
	o<<t.data;
	if(t.b) o<<"b";
	return o;
}

bool operator<(Team const& a,Team const& b){
	if(a.data<b.data) return 1;
	if(a.data>b.data) return 0;
	return a.b<b.b;
}

bool operator==(Team const& a,Team const& b){
	return a.data==b.data && a.b==b.b;
}
