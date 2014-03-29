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

//turns all runs of whitespace into a single space
string merge_whitespace(string const& in){
	stringstream ss;
	bool last_was=1;
	for(auto c:in){
		if(isblank(c)){
			if(last_was) continue;
			ss<<' ';
			last_was=1;
		}else{
			ss<<c;
			last_was=0;
		}
	}
	return ss.str();
}

vector<string> split(string const& s){
	return split(merge_whitespace(s),' ');
}

string toupper(string const& s){
	stringstream ss;
	for(auto c:s){
		ss<<(char)::toupper(c);
	}
	return ss.str();
}

string tolower(string const& s){
	stringstream ss;
	for(auto c:s){
		ss<<(char)::tolower(c);
	}
	return ss.str();
}

