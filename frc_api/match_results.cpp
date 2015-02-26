#include "match_results.h"
#include "../util.h"

using namespace std;

/*#ifndef MATCH_RESULTS_H
#define MATCH_RESULTS_H

#define ALLIANCE_STATION_OPTIONS X(red1) X(red2) X(red3) X(blue1) X(blue2) X(blue3)

class Alliance_station{
	string data;

	public:
	explicit Alliance_station(string s):data(s){
		#define X(NAME) if(s==""#NAME) return;
		ALLIANCE_STATION_OPTIONS
		#undef X
		assert(0);
	}

	string const& get()const{ return data; }
};
*/
ostream& operator<<(ostream& o,Alliance_station const& a){
	return o<<a.get();
}
/*
typedef string Datetime;//this should eventually do more

enum class Tournament_level{QUALIFICATION,PLAYOFF};
*/

ostream& operator<<(ostream& o,Tournament_level a){
	if(a==Tournament_level::QUALIFICATION) return o<<"qual";
	if(a==Tournament_level::PLAYOFF) return o<<"playoff";
	assert(0);
}

/*
typedef unsigned int Match_number;

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
*/
ostream& operator<<(ostream& o,Match_results::Team const& a){
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
/*
class Match_results_query{
	public:
	struct Data{
		Season season;
		Event_code event_code;
		Maybe<Tournament_level> tournament_level;
		Maybe<Team_number> team_number;
		Maybe<Match_number> match_number;
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

class Add_optional{
	bool has_q=0;
	ostream& o;
*/
void Add_optional::add_prefix(){
	if(has_q){
		o<<"?";
		has_q=1;
	}else{
		o<<"&";
	}
}

Add_optional::Add_optional(ostream& o1):has_q(0),o(o1){}
/*
	template<typename T>
	void operator()(string name,Maybe<T> t){
		if(t){
			add_prefix();
			o<<name<<"="<<*t;
		}
	}
*/
void Add_optional::operator()(string name){
	add_prefix();
	o<<name;
}

ostream& operator<<(ostream& o,Match_results_query const& a){
	o<<"/api/v1.0/matches/"<<a.data().season<<"/"<<a.data().event_code;
	Add_optional add_optional(o);
	#define X(NAME) add_optional(""#NAME,a.data().NAME);
	X(tournament_level)
	X(team_number)
	X(match_number)
	X(start)
	X(end)
	#undef X
	return o;
}
