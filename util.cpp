#include "util.h"
#include<fcntl.h>
#include<cassert>
#include<unistd.h>

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
