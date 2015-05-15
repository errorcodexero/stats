#include "event_awards.h"

using namespace std;

void init(int &x){ x=0; }
void init(string&){}

template<typename T>
void init(Maybe<T> const&){}

Event_awards::Event_awards(){
	#define X(A,B) init(B);
	EVENT_AWARD_ITEMS
	#undef X
}

ostream& operator<<(ostream& o,Event_awards const& a){
	o<<"Event_awards(";
	#define X(a1,b) o<<""#b<<":"<<a.b<<" ";
	EVENT_AWARD_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Event_awards_query const& a){
	o<<"/api/1.0/awards/"<<a.season<<"/?";
	if(a.data.a()){
		o<<a.data.a();
		if(a.data.b()){
			o<<"&"<<a.data.b();
		}
	}else{
		o<<a.data.b();
	}
	return o;
}
