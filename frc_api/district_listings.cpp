#ifndef DISTRICT_LISTINGS_H
#define DISTRICT_LISTINGS_H

#define DISTRICT_ITEMS\
	X(District_code,code)\
	X(string,name)

struct District{
	#define X(A,B) A B;
	DISTRICT_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,District const& a){
	o<<"District(";
	#define X(A,B) o<<a.B<<" ";
	DISTRICT_ITEMS
	#undef X
	return o<<")";
}

#define DISTRICT_LISTINGS_ITEMS\
	X(unsigned,districtCount)\
	X(vector<District>,districts)

struct District_listings{
	#define X(A,B) A B;
	DISTRICT_LISTINGS_ITEMS
	#undef X
};

ostream& operator<<(ostream& o,District_listings const& a){
	o<<"District_listings(";
	#define X(A,B) o<<a.B<<" ";
	DISTRICT_LISTINGS_ITEMS
	#undef X
	return o<<")";
}

struct District_query{
	Season season;
};

ostream& operator<<(ostream& o,District_query a){
	return o<<"/api/v1.0/districts/"<<a.season;
}

#endif