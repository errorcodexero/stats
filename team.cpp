#include "team.h"
#include<iostream>
#include<cassert>
#include "util.h"

using namespace std;

static const int MAX_TEAM_NUMBER=9999;

Team::Team(string a){
	if(prefix(a,"frc")) a=a.substr(3,a.size()-3);
	assert(a.size());
	char c=a[a.size()-1];
	bool b=0;
	if(c=='b'){
		b=1;
		a=a.substr(0,a.size()-1);
	}
	data=atoi(a.c_str());
	assert(data);
	if(b) data|=0x4000;

	assert(num() && num()<=MAX_TEAM_NUMBER);
}

//There are some places in The Blue Alliance data where the team # is recorded as 9999.  This is obviously wrong.  For now, we're just letting them stay as team 9999.  But in the long term, we should really figure out what that means.  

short Team::num()const{ return data&0x3fff; }
bool Team::b()const{ return data&0x4000; }

ostream& operator<<(ostream& o,Team t){
	o<<t.num();
	if(t.b()) o<<"b";
	return o;
}

bool operator<(Team a,Team b){ return a.data<b.data; }

bool operator>(Team a,Team b){
	return b<a;
}

bool operator==(Team a,Team b){
	return a.data==b.data;
}

bool operator!=(Team a,Team b){
	return !(a==b);
}
