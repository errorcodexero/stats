#ifndef MAIN_H
#define MAIN_H

typedef unsigned U;
typedef std::tuple<Team,Team,U,U,U,U,U,U> Flat_head_to_head;
std::vector<Flat_head_to_head> flat_head_to_head(std::vector<Match_info> const&);
double average_score(int year);
double median_score(int year);

#endif
