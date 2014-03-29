#include "record.h"
#include<iostream>
#include "util.h"

using namespace std;

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

Record::Record():win(0),loss(0),tie(0){}
Record::Record(unsigned a,unsigned b,unsigned c):win(a),loss(b),tie(c){}

Record& Record::operator+=(Record a){
	win+=a.win;
	loss+=a.loss;
	tie+=a.tie;
	return *this;
}

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

Record reversed(Record a){
	return Record(a.loss,a.win,a.tie);
}

bool operator==(Record a,Record b){
	return a.win==b.win && a.loss==b.loss && a.tie==b.tie;
}

bool operator<(Record a,Record b){
	auto ap=points(a),bp=points(b);
	#define X(v1,v2) if(v1<v2) return 1; if(v2<v1) return 0;
	X(ap,bp);
	X(a.win,b.win)
	#undef X
	return a.loss>b.loss;
}

ostream& operator<<(ostream& o,Record r){
	o<<r.win<<"-"<<r.loss;
	if(r.tie) o<<"-"<<r.tie;
	return o;
}
