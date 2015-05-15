#include "ancillary.h"
#include<iostream>

std::ostream& operator<<(std::ostream& o,Ancillary const& a){
	o<<"Ancillary(";
	o<<a.name<<" "<<a.version<<" "<<a.status;
	return o<<")";
}

const std::string ANCILLARY_QUERY="/api/v1.0/";
