#include "exception.h"
#include "util.h"

using namespace std;

#define T File_not_found
T::T(std::string const&s){
	stringstream ss;
	ss<<"File not found "<<s;
	msg=ss.str();
}
T::~T()throw(){}

const char *T::what()const throw(){
	return msg.c_str();
}
#undef T

#define T Value_not_found
T::T()nyi

T::T(string const& s){
	stringstream ss;
	ss<<"Value not found \""<<s<<"\"";
	msg=ss.str();
}

T::T(T const& t):msg(t.msg){}
T::~T()throw(){}

const char *T::what()const throw(){
	return msg.c_str();
}

ostream& operator<<(ostream& o,T const& t)nyi
#undef T
