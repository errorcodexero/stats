#include "season_data.h"

using namespace std;

ostream& operator<<(ostream& o,Season_data const& a){
	o<<"Season_data(";
	#define X(A,B) o<<a.B<<" ";
	SEASON_DATA_ITEMS
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Season_data_query a){
	return o<<"/api/v1.0/"<<a.season;
}
