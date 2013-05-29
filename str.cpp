#include "str.h"
#include "util.h"

using namespace std;

vector<string> split(string const& s,char c){
	vector<string> r;
	auto at=s.begin(),end=s.end();
	while(at!=end){
		auto begin=at;
		while(at!=end && *at!=c){
			at++;
		}
		r|=string(begin,at);
		if(at!=end){
			at++;
		}
	}
	return r;
}


