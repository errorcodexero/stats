#ifndef MATCH_RESULTS_H
#define MATCH_RESULTS_H

#include<vector>
#include "common.h"
#include "event_alliances.h"

#define ALLIANCE_STATION_OPTIONS X(red1) X(red2) X(red3) X(blue1) X(blue2) X(blue3)

class Alliance_station{
	std::string data;

	public:
	explicit Alliance_station(std::string);
	std::string const& get()const{ return data; }
};

std::ostream& operator<<(std::ostream&,Alliance_station const&);

typedef std::string Datetime;//this should eventually do more

enum class Tournament_level{QUALIFICATION,PLAYOFF};

std::ostream& operator<<(std::ostream&,Tournament_level);

typedef unsigned int Match_number;

#define MATCH_RESULTS_ITEMS \
	X(Datetime,actualStartTime)\
	X(std::string,description)\
	X(Tournament_level,level)\
	X(Match_number,matchNumber)\
	X(int,scoreRedFinal)\
	X(int,scoreRedFoul)\
	X(int,scoreRedAuto)\
	X(int,scoreBlueFinal)\
	X(int,scoreBlueFoul)\
	X(int,scoreBlueAuto)\
	X(std::vector<Match_results::Team>,teams)

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

std::ostream& operator<<(std::ostream&,Match_results::Team const&);
std::ostream& operator<<(std::ostream&,Match_results const&);

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
	std::ostream& o;

	void add_prefix();

	public:
	Add_optional(std::ostream&);

	template<typename T>
	void operator()(std::string name,Maybe<T> t){
		if(t){
			add_prefix();
			o<<name<<"="<<*t;
		}
	}

	void operator()(std::string name);
};

std::ostream& operator<<(std::ostream&,Match_results_query const&);

#endif
