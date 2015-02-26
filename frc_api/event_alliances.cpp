#include "event_alliances.h"
#include "../util.h"
#include "../str.h"

using namespace std;
/*
#ifndef EVENT_ALLIANCES_H
#define EVENT_ALLIANCES_H

class Alliance_count{
	int value;

	public:
*/
Alliance_count::Alliance_count(int x):value(x){
	assert(value==4 || value==8);
}

int Alliance_count::get()const{ return value; }

ostream& operator<<(ostream& o,Alliance_count const& a){
	return o<<a.get();
}

/*struct Event_alliances{
	Alliance_count count;

	struct Alliance{
		int number;//1-4 or 1-8
		Team_number captain,round1,round2;
		Maybe<Team_number> round3,backup;
		Maybe<Team_number> backupReplaced;
	};

	vector<Alliance> alliances;
};
*/
ostream& operator<<(ostream& o,Event_alliances::Alliance const& a){
	o<<"Alliance( ";
	#define X(NAME) o<<a.NAME<<" ";
	X(number) X(captain) X(round1) X(round2)
	X(round3) X(backup) X(backupReplaced)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_alliances const& a){
	o<<"Event_alliances(";
	o<<a.count;
	o<<a.alliances;
	return o<<")";
}

ostream& operator<<(ostream& o,Season){
	return o<<"2015";
}

/*class Event_code{
	//this is normalized to lowercase, event though the API is case-insensitive
	string s;

	public:*/
Event_code::Event_code(string s1):s(tolower(s1)){
	assert(s.size()>2);
}

string const& Event_code::get()const{ return s; }

ostream& operator<<(ostream& o,Event_code const& a){
	return o<<a.get();
}
/*
struct Alliance_query{
	Season season;
	Event_code event_code;
};*/

ostream& operator<<(ostream& o,Alliance_query const& a){
	return o<<"/api/v1.0/alliances/"<<a.season<<"/"<<a.event_code;
}
