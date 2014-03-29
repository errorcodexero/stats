#ifndef CALCULATE_H
#define CALCULATE_H

#include<map>
#include<vector>
#include "record.h"

struct Match_info;
struct Team;

std::map<Team,Record> calculate_records(std::vector<Match_info> const&);

Record tally(Match_result);
Record tally(std::vector<Match_result> const&);

#endif
