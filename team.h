#ifndef TEAM_H
#define TEAM_H

#include<string>

class Team{
	short data;

	short num()const;
	bool b()const;

	public:
	explicit Team(std::string);

	friend std::ostream& operator<<(std::ostream&,Team);
	friend bool operator<(Team,Team);
	friend bool operator==(Team,Team);
};

std::ostream& operator<<(std::ostream&,Team);
bool operator<(Team,Team);
bool operator>(Team,Team);
bool operator==(Team,Team);
bool operator!=(Team,Team);

#endif
