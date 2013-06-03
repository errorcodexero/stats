#include "team.h"
#include<iostream>
#include<cassert>
#include "util.h"

using namespace std;

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
}

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
