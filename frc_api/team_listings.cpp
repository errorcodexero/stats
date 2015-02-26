#ifndef TEAM_LISTINGS_H
#define TEAM_LISTINGS_H

typedef int Rookie_year;

#define TEAM_ITEMS\
	X(Team_number,teamNumber)\
	X(string,nameFull)\
	X(string,nameShort)\
	X(string,city)\
	X(string,stateprov)\
	X(string,country)\
	X(Rookie_year,rookieYear)\
	X(string,robotName)\
	X(Maybe<District_code>,districtCode)

struct Team{
	#define X(A,B) A B;
	TEAM_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Team const& a){
	o<<"Team(";
	#define X(A,B) o<<a.B<<" ";
	TEAM_ITEMS
	#undef X
	return o<<")";
}

#define TEAM_LISTINGS_ITEMS\
	X(int,pageCurrent)\
	X(int,pageTotal)\
	X(int,teamCountPage)\
	X(int,teamCountTotal)\
	X(vector<Team>,teams)

struct Team_listings{
	#define X(A,B) A B;
	TEAM_LISTINGS_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,Team_listings const& a){
	o<<"Team_listings(";
	#define X(A,B) o<<a.B<<" ";
	return o<<")";
}

struct Team_listings_query{
	Season season;
	At_most_one<Team_number,pair<Maybe<Event_code>,Maybe<District_code>>> restrictions;
	Maybe<int> page;
};

ostream& operator<<(ostream& o,Team_listings_query const& a){
	o<<"/api/v1.0/teams/"<<a.season;
	Add_optional opt(o);
	opt("teamNumber",a.restrictions.a());
	if(a.restrictions.b()){
		auto x=*a.restrictions.b();
		opt("eventCode",x.first);
		opt("districtCode",x.second);
	}
	opt("page",a.page);
	return o;
}

#endif