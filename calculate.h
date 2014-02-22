#ifndef CALCULATE_H
#define CALCULATE_H

#include<map>
#include<vector>

struct Match_info;
struct Team;
struct Record;

std::map<Team,Record> calculate_records(std::vector<Match_info> const&);

#endif