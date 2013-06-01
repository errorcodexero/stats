#include "util.h"
#include<fcntl.h>
#include<cassert>
#include<unistd.h>
#include<algorithm>
#include "default.h"

using namespace std;

void tab(std::ostream& o,unsigned i){
	while(i--) o<<"\t";
}

std::string slurp(std::string const& filename){
	std::ifstream f(filename.c_str());
	if(!f.good()){
		throw File_not_found(filename);
	}
	std::stringstream ss;
	while(f>>ss.rdbuf());
	return ss.str();
}

void write_out(string const& filename,string const& data){
	int mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;//the permissions for the file if a new one is created.  
	int fd=open(filename.c_str(),O_WRONLY|O_CREAT|O_TRUNC,mode);
	assert(fd!=-1);
	unsigned i=0;
	while(i<data.size()){
		auto r=write(fd,data.c_str()+i,data.size()-i);
		assert(r>0);
		i+=r;
	}
	int r=close(fd);
	assert(!r);
}

bool prefix(string const& in,string const& val){
	if(in.size()<val.size()) return 0;
	for(unsigned i=0;i<val.size();i++){
		if(in[i]!=val[i]) return 0;
	}
	return 1;
}

string cat(string const& a,string const& b){
	stringstream ss;
	ss<<a<<b;
	return ss.str();
}

string cat(string const& a,char b){
	stringstream ss;
	ss<<a<<b;
	return ss.str();
}

string cat(char a,char b){
	stringstream ss;
	ss<<a<<b;
	return ss.str();
}

vector<string> lines(string const& s){
	vector<string> r;
	const char *at=s.c_str(),*begin=s.c_str();
	while(*at){
		while(*at && *at!='\n'){
			at++;
		}
		if(*at=='\n'){
			r.push_back(string(begin,at));
			at++;
			begin=at;
		}
	}
	r.push_back(string(begin,at));
	return r;
}

double sum(vector<int> const& v){
	double t=0;
	for(auto a:v) t+=a;
	return t;
}

Maybe<double> mean(vector<int> const& v){
	if(!v.size()) return Maybe<double>();
	return sum(v)/v.size();
}

Maybe<double> median(vector<int> v){
	if(!v.size()) return Maybe<double>();
	sort(begin(v),end(v));
	return v[v.size()/2];//could do the right thing when length is divisible by 2.
}

Maybe<int> mode(vector<int> const& v){
	if(!v.size()) return Maybe<int>();
	map<int,Default<int,0>> m;
	for(auto a:v) m[a]++;
	vector<pair<int,int>> vout;
	for(auto p:m){
		vout|=make_pair((int)p.second,p.first);
	}
	sort(begin(vout),end(vout));
	return vout[vout.size()-1].second;
}

std::string join(std::vector<std::string> const& v,char c){
	std::stringstream ss;
	for(unsigned i=0;i+1<v.size();i++){
		ss<<v[i]<<c;
	}
	if(v.size()){
		ss<<v[v.size()-1];
	}
	return ss.str();
}
