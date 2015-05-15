#include<iostream>
#include<cassert>
#include "../maybe.h"
#include "../util.h"
#include "../str.h"
#include "ancillary.h"
#include "event_alliances.h"
#include "event_awards.h"
#include "award_listings.h"
#include "match_results.h"
#include "event_rankings.h"
#include "event_schedule.h"
#include "hybrid_schedule.h"
#include "season_data.h"
#include "event_listing.h"
#include "district_listings.h"
#include "team_listings.h"

using namespace std;

int main(){
	{
		Ancillary a;
		cout<<a<<"\n";
		cout<<ANCILLARY_QUERY;	
	}

	static const Event_code evt{"orwil"};
	{
		Event_alliances a{Alliance_count{8},{}};
		cout<<a<<"\n";

		Alliance_query q{Season{},evt};
		cout<<q<<"\n";
	}

	{
		Event_awards a;
		cout<<a<<"\n";

		Event_awards_query q{Season{},At_least_one<Event_code,Team_number>{evt}};
		cout<<q<<"\n";
	}

	{
		Award_listings a{0,Event_type::REGIONAL,"",0};
		cout<<a<<"\n";

		cout<<award_query(Season{})<<"\n";
	}

	{
		Match_results e{
			Datetime{},"",Tournament_level::PLAYOFF,Match_number{},
			int{},int{},int{},
			int{},int{},int{},
			{}
		};
		cout<<e<<"\n";

		Match_results_query q{Match_results_query::Data{
			Season{},
			evt,
			Tournament_level::QUALIFICATION,
			Maybe<Team_number>{},
			Maybe<Match_number>{},Maybe<Match_number>{},Maybe<Match_number>{}
		}};
		cout<<q<<"\n";
	}

	Team_number team_number{122};
	{
		Event_rankings a{
			0,0,0,0,
			0,0,0,
			Positive_int(4),
			team_number,
			0,0,0,0
		};
		cout<<a<<"\n";

		Event_rankings_query q{Season{},evt,Maybe<Team_number>{},0};
		cout<<q<<"\n";
	}

	{
		Event_schedule a{"",Tournament_level::QUALIFICATION,3,Datetime{},{}};
		cout<<a<<"\n";

		Event_schedule_query b{
			Season{},evt,At_least_one<Tournament_level,Team_number>{team_number},Maybe<Match_number>{},Maybe<Match_number>{}
		};
		cout<<b<<"\n";
	}
	auto level=Tournament_level::PLAYOFF;
	{
		Hybrid_schedule a{"",level,11,Datetime{},0,0,0,0,0,0,{}};
		cout<<a<<"\n";

		Hybrid_schedule_query b{Season{},evt,level,Maybe<Match_number>{},Maybe<Match_number>{}};
		cout<<b<<"\n";
	}

	{
		Season_data a{Datetime{},34,"",Datetime{},team_number,2800};
		cout<<a<<"\n";

		Season_data_query q;
		cout<<q<<"\n";
	}

	{
		Event_listing a;
		cout<<a<<"\n";

		Event_listing_query b{Event_listing_query::Data{
			Season{},Maybe<Event_code>{},Maybe<Team_number>{},Maybe<District_code>{},Maybe<bool>{}
		}};
		cout<<b<<"\n";
	}

	/*#include "district_listings.h"
	#include "team_listings.h"*/
	{
		District_listings a{0,{}};
		cout<<a<<"\n";

		District_query q;
		cout<<q<<"\n";
	}

	{
		Team_listings t{1,1,1,1,{}};
		cout<<t<<"\n";

		Team_listings_query q;
		cout<<q<<"\n";
	}
	return 0;
}
