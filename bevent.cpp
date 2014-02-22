#include "bevent.h"
#include<iostream>

using namespace std;

ostream& operator<<(ostream& o,BEvent const& b){
	o<<"BEvent("<<b.name<<" ";
	o<<"name_short:"<<b.name_short<<" ";
	o<<"start_date:"<<b.start_date<<" ";
	o<<"end_date:"<<b.end_date<<" ";
	o<<"official:"<<b.official<<" ";
	o<<b.key<<")";
	return o;
}
