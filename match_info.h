#ifndef MATCH_INFO_H
#define MATCH_INFO_H

#include<set>
#include<vector>
#include<map>
#include<string>
#include "maybe.h"

class Team;

enum class Competition_level{
	QUALS,
	EIGHTHS, //This actually shows up in some of the The Blue Alliance data.
	QUARTERS,SEMIS,FINALS
};

Maybe<Competition_level> parse_competition_level(std::string);
std::ostream& operator<<(std::ostream& o,Competition_level);

typedef std::string Event_key;

struct Match_info{
	int match_number;
	int set_number;
	Competition_level competition_level;
	std::string key;
	std::set<Team> teams; //warning: This is not accurate when just parsed in because doesn't work with "B" teams
	struct Alliance{
		int score;
		std::vector<Team> teams;
	};
	std::map<std::string,Alliance> alliances;
	Event_key event;
};

std::ostream& operator<<(std::ostream&,Match_info::Alliance const&);
std::ostream& operator<<(std::ostream&,Match_info const&);

bool operator<(Match_info::Alliance const&,Match_info::Alliance const&);

std::set<Competition_level> competition_level(std::vector<Match_info> const&);

bool ok(Match_info const&);

std::set<Team> teams(Match_info const&);
std::set<Team> teams(std::vector<Match_info> const&);
std::vector<int> scores(std::vector<Match_info> const&);
std::vector<Match_info::Alliance> winning_alliances(std::vector<Match_info> const& matches);
std::vector<Match_info::Alliance> losing_alliances(std::vector<Match_info> const& m);
Maybe<double> mean_score(std::vector<Match_info::Alliance> const& v);
std::vector<Match_info::Alliance> alliances(std::vector<Match_info> const&);
std::vector<Match_info> with_team(std::vector<Match_info> const&,Team);
std::vector<Match_info> with_event(std::vector<Match_info> const& m,Event_key);
std::set<Event_key> events(std::vector<Match_info> const&);
std::vector<Match_info> eliminations(std::vector<Match_info>);
std::vector<Match_info> finals(std::vector<Match_info>);
std::vector<Match_info> quals(std::vector<Match_info>);

#endif
