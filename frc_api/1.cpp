#include<iostream>
#include<cassert>
#include "../maybe.h"
#include "../util.h"
#include "../str.h"

using namespace std;

class Team_number{
	int x;

	public:
	explicit Team_number(int v):x(v){
		assert(x>0 && x<6000);
	}

	int get()const{ return x; }
};

ostream& operator<<(ostream& o,Team_number a){
	return o<<"team"<<a.get();
}

class Alliance_count{
	int value;

	public:
	explicit Alliance_count(int x):value(x){
		assert(value==4 || value==8);
	}

	int get()const{ return value; }
};

ostream& operator<<(ostream& o,Alliance_count const& a){
	return o<<a.get();
}

struct Event_alliances{
	Alliance_count count;

	struct Alliance{
		int number;//1-4 or 1-8
		Team_number captain,round1,round2;
		Maybe<Team_number> round3,backup;
		Maybe<Team_number> backupReplaced;
	};

	vector<Alliance> alliances;
};

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

class Season{}; //only one value allowed for now

ostream& operator<<(ostream& o,Season){
	return o<<"2015";
}

class Event_code{
	//this is normalized to lowercase, event though the API is case-insensitive
	string s;

	public:
	explicit Event_code(string s1):s(tolower(s1)){
		assert(s.size()>2);
		for(auto c:s) assert(isalpha(c));
	}
};

ostream& operator<<(ostream& o,Event_code const& a){
	nyi
}

struct Alliance_query{
	Season season;
	Event_code event_code;
};

ostream& operator<<(ostream& o,Alliance_query const& a){
	return o<<"/api/v1.0/alliances/"<<a.season<<"/"<<a.event_code;
}

struct Ancillary{
	string name,version,status;
};

static const string ANCILLARY_QUERY="/api/v1.0/";

#define EVENT_AWARD_ITEMS \
	X(int,awardId)\
	X(int,teamId)\
	X(int,eventId)\
	X(int,eventDivisionId)\
	X(string,name)\
	X(int,series)\
	X(Maybe<Team_number>,teamNumber)\
	X(Maybe<string>,person)

struct Event_awards{
	#define X(a,b) a b;
	EVENT_AWARD_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Event_awards const& a){
	o<<"Event_awards(";
	#define X(a1,b) o<<a.b<<" ";
	EVENT_AWARD_ITEMS
	#undef X
	return o<<")";
}

template<typename A,typename B>
class At_least_one{
	Maybe<A> a_;
	Maybe<B> b_;

	public:
	explicit At_least_one(A a):a_(a){}
	explicit At_least_one(B b):b_(b){}
	explicit At_least_one(A a,B b):a_(a),b_(b){}

	Maybe<A> a()const{ return a_; }
	Maybe<B> b()const{ return b_; }
};

template<typename A,typename B>
ostream& operator<<(ostream& o,At_least_one<A,B> const& a){
	o<<"At_least_one(";
	o<<a.a()<<a.b();
	return o<<")";
}

struct Event_awards_query{
	Season season;
	At_least_one<Event_code,Team_number> data;
};

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

enum class Event_type{
	NONE,
	REGIONAL,
	DISTRICT_EVENT,
	DISTRICT_CHAMPIONSHIP,
	CHAMPIONSHIP_SUBDIVISION,
	CHAMPIONSHIP_DIVISION,
	CHAMPIONSHIP
};

ostream& operator<<(ostream& o,Event_type){
	nyi
}

#define AWARD_LISTING_ITEMS \
	X(int,awardId)\
	X(Event_type,eventType)\
	X(string,description)\
	X(bool,forPerson)

struct Award_listings{
	#define X(A,B) A B;
	AWARD_LISTING_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Award_listings const& a){
	o<<"Award_listings(";
	#define X(A,B) o<<a.B<<" ";
	AWARD_LISTING_ITEMS
	#undef X
	return o<<")";
}

string award_query(Season season){
	stringstream ss;
	ss<<"/api/v1.0/awards/"<<season<<"/list";
	return ss.str();
}

class Alliance_station{

	public:
	explicit Alliance_station(string s){
		nyi
	}
};

ostream& operator<<(ostream& o,Alliance_station)nyi

typedef string Datetime;//this should eventually do more

enum class Tournament_level{QUALIFICATION,PLAYOFF};

ostream& operator<<(ostream& o,Tournament_level)nyi

typedef int Match_number;

#define MATCH_RESULTS_ITEMS \
	X(Datetime,actualStartTime)\
	X(string,description)\
	X(Tournament_level,level)\
	X(Match_number,matchNumber)\
	X(int,scoreRedFinal)\
	X(int,scoreRedFoul)\
	X(int,scoreRedAuto)\
	X(int,scoreBlueFinal)\
	X(int,scoreBlueFoul)\
	X(int,scoreBlueAuto)\
	X(vector<Match_results::Team>,teams)

struct Match_results{
	struct Team{
		Team_number teamNumber;
		Alliance_station station;
		bool dq;
	};

	#define X(A,B) A B;
	MATCH_RESULTS_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Match_results::Team a){
	o<<"Match_results::Team(";
	o<<a.teamNumber<<" ";
	o<<a.station<<" ";
	o<<a.dq;
	return o<<")";
}

ostream& operator<<(ostream& o,Match_results const& a){
	o<<"Match_results(";
	#define X(A,B) o<<" "<<a.B;
	MATCH_RESULTS_ITEMS
	#undef X
	return o<<")";
}

class Match_results_query{
	public:
	struct Data{
		Season season;
		Event_code event_code;
		Maybe<Tournament_level> tournament_level;
		Maybe<Team_number> team_number;
		Match_number match_number;
		Maybe<Match_number> start,end;
	};

	private:
	Data data_;

	public:
	Match_results_query(Data d):data_(d){
		if(d.match_number || d.start || d.end){
			assert(d.tournament_level);
		}
		assert(!d.team_number || !d.match_number);
		assert(!d.match_number || (!d.start && !d.end));
	}

	Data data()const{ return data_; }
};

ostream& operator<<(ostream& o,Match_results_query const& a){
	o<<"/api/v1.0/matches/"<<a.data().season<<"/"<<a.data().event_code;
	bool has_and=0;
	auto add_optional=[&](auto value){
		if(value){
			if(has_and){
				nyi
			}else{
				nyi
			}
		}
	};
	add_optional(a.data().tournament_level);
	add_optional(a.data().team_number);
	add_optional(a.data().match_number);
	add_optional(a.data().start);
	add_optional(a.data().end);
	return o;
}

struct Event_rankings{
	
};

ostream& operator<<(ostream& o,Event_rankings const& a){
	nyi
}

struct Event_rankings_query{
	Season season;
	Event_code event_code;
	Maybe<Team_number> team_number;
	bool top;
};

ostream& operator<<(ostream& o,Event_rankings_query const&){
	nyi
}

int main(){
	
}
