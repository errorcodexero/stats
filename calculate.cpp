#include "calculate.h"
#include "match_info.h"
#include "map.h"
#include "util.h"
#include "record.h"
#include "team.h"

using namespace std;

map<Team,Record> calculate_records(vector<Match_info> const& m){
	map<Team,Record> r;
	for(Match_info const& match:m){
		auto v=values(match.alliances);
		auto scores=mapf([](Match_info::Alliance a){ return a.score; },v);
		assert(scores.size()==2); //not dealing with more than two alliances at the moment.  
		if(scores[0]==scores[1]){
			auto tie=[&](Team t){ return r[t].tie++; };
			mapf(tie,teams(match));
		}else{
			auto win=[&](Team t){ return r[t].win++; };
			auto loss=[&](Team t){ return r[t].loss++; };
			auto teams=mapf([](Match_info::Alliance a){ return a.teams; },v);
			if(scores[0]>scores[1]){
				mapf(win,teams[0]);
				mapf(loss,teams[1]);
			}else{
				mapf(loss,teams[0]);
				mapf(win,teams[1]);
			}
		}
	}
	return r;
}

Record tally(Match_result m){
	switch(m){
		case Match_result::WIN: return Record{1,0,0};
		case Match_result::LOSS: return Record{0,1,0};
		case Match_result::TIE: return Record{0,0,1};
		default:nyi
	}
}

Record tally(vector<Match_result> const& v){
	Record r;
	for(auto a:v){
		switch(a){
			case Match_result::WIN:
				r.win++;
				break;
			case Match_result::LOSS:
				r.loss++;
				break;
			case Match_result::TIE:
				r.tie++;
				break;
			default:
				assert(false);
		}
	}
	return r;
}