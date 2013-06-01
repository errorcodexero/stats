#include "record.h"
#include<iostream>

using namespace std;

Record::Record():win(0),loss(0),tie(0){}

unsigned sum(Record r){
	return r.win+r.loss+r.tie;
}

//I know that this isn't how the rankings have worked every year, but we're going to go with it.  
unsigned points(Record r){
	return 2*r.win+r.tie;
}

//Returns 0% on no data.
double win_portion(Record r){
	auto s=sum(r);
	if(!s) return 0;
	return .5*points(r)/s;
}

bool operator<(Record a,Record b){
	return points(a)<points(b);
}

ostream& operator<<(ostream& o,Record r){
	o<<r.win<<"-"<<r.loss;
	if(r.tie) o<<"-"<<r.tie;
	return o;
}
