#ifndef PICK_H
#define PICK_H

#include<array>
#include<map>
#include "team.h"

enum class Alliance_result{W,F,SF,QF};
std::ostream& operator<<(std::ostream&,Alliance_result);

struct Alliance{
	std::array<Team,3> teams;
	Alliance_result result;
};
std::ostream& operator<<(std::ostream&,Alliance);

typedef std::array<Alliance,8> Event_picks;

std::map<std::string,Event_picks> parse_picks();

#endif
