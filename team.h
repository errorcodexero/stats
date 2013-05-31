#ifndef TEAM_H
#define TEAM_H

#include<string>

class Team{
	unsigned data;
	bool b;

	public:
	explicit Team(std::string);

	friend std::ostream& operator<<(std::ostream&,Team const&);
	friend bool operator<(Team const&,Team const&);
	friend bool operator==(Team const&,Team const&);
};

std::ostream& operator<<(std::ostream&,Team const&);
bool operator<(Team const&,Team const&);
bool operator==(Team const&,Team const&);

#endif