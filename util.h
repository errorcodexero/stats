#ifndef UTIL_H
#define UTIL_H

#include<fstream>
#include<sstream>
#include<iostream>
#include<stdlib.h>
#include<map>
#include<vector>
#include<set>
#include<algorithm>
#include<array>
#include "exception.h"
#include "maybe.h"
#include "default.h"

#define STREAM_OUT std::ostream& operator<<(std::ostream&,T const&);
#define NYI {\
	std::cerr.flush();\
	std::cout.flush();\
	std::cout<<"NYI "<<__FILE__<<":"<<__LINE__<<"\n";\
	exit(2);\
	}
#define nyi NYI

#define PRINT(x) { std::cout<<(#x)<<"="<<(x)<<"\n"; }
#define PRINT_TO(dest,val) { dest<<#val<<"="<<val; }

#define RM_REF(X) typename remove_reference<X>::type
#define RM_CONST(X) typename remove_const<X>::type
#define ELEMENT(X) RM_CONST(RM_REF(decltype(*begin(X))))

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::pair<A,B> const& p){
	o<<"pair("<<p.first<<","<<p.second<<")";
	return o;
}

template<typename K,typename V>
std::ostream& operator<<(std::ostream& o,std::map<K,V> const& m){
	o<<"{";
	for(auto at=m.begin();at!=m.end();at++){
		o<<*at;
		auto next=at;
		next++;
		if(next!=m.end()){
			o<<",";
		}
	}
	return o<<"}";
}

template<typename T>
std::vector<T>& operator|=(std::vector<T>& v,T t){
	v.push_back(t);
	return v;
}

template<typename T>
std::vector<T>& operator|=(std::vector<T> &v,std::vector<T> a){
	for(auto elem:a) v|=elem;
	return v;
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::vector<T> const& v){
	o<<"[";
	for(unsigned i=0;i<v.size();i++){
		o<<v[i];
		if(i+1<v.size()) o<<",";
	}
	return o<<"]";
}

std::string join(std::vector<std::string> const&,char);

//aka cdr
template<typename T>
std::vector<T> tail(std::vector<T> const& v){
	std::vector<T> r;
	for(unsigned i=1;i<v.size();i++){
		r|=v[i];
	}
	return r;
}

template<typename T>
std::string as_string(T const& t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

template<typename Func,typename Collection>
auto mapf(Func f,Collection const& c)->std::vector<decltype(f(*begin(c)))>{
	std::vector<decltype(f(*begin(c)))> r;
	for(auto elem:c) r|=f(elem);
	return r;
}

template<typename Func,typename Collection>
Collection filter(Func f,Collection const& in){
	Collection r;
	for(auto elem:in){
		if(f(elem)) r|=elem;
	}
	return r;
}

template<typename T>
Maybe<T> operator&(std::set<T> s,T t){
	if(contains(s,t)){
		return t;
	}
	return Maybe<T>();
}

template<typename T>
std::vector<T> take(unsigned lim,std::vector<T> const& in){
	std::vector<T> r;
	for(unsigned i=0;i<lim;i++){
		r|=in[i];
	}
	return r;
}

template<typename T>
std::vector<T> skip(unsigned i,std::vector<T> const& v){
	std::vector<T> r;
	for(;i<v.size();i++) r|=v[i];
	return r;
}

template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> const& v){
	std::vector<T> r;
	for(auto a:v) for(auto elem:a) r|=elem;
	return r;
}

template<typename T>
bool contains(std::vector<T> const& v,T t){
	for(auto a:v){
		if(a==t) return 1;
	}
	return 0;
}

template<typename T>
T max(std::vector<T> const& v){
	T r=*begin(v);
	for(auto a:v) r=std::max(r,a);
	return r;
}

template<typename T>
T min(std::vector<T> const& v){
	T r=*begin(v);
	for(auto a:v) r=std::min(r,a);
	return r;
}

template<typename T,unsigned N>
std::ostream& operator<<(std::ostream& o,std::array<T,N> a){
	o<<"[ ";
	for(auto elem:a) o<<elem<<" ";
	return o<<"]";
}

//could also do a multiset...
template<typename T>
std::map<T,Default<unsigned,0>> count(std::vector<T> v){
	std::map<T,Default<unsigned,0>> r;
	for(auto a:v) r[a]++;
	return r;
}

std::string tag(std::string const&,std::string const&);

void tab(std::ostream& o,unsigned i);
std::string slurp(std::string const& filename);
void write_out(std::string const& filename,std::string const& data);
bool prefix(std::string const& in,std::string const& val);
std::string cat(std::string const&,std::string const&);
std::string cat(std::string const&,char);
std::string cat(char,char);
std::vector<std::string> lines(std::string const&);
Maybe<double> mean(std::vector<int> const&);
Maybe<double> median(std::vector<int>);
Maybe<int> mode(std::vector<int> const&);
std::vector<int> quartiles(std::vector<int>);

template<typename T>
T sum(std::vector<T> v){
	T total{};
	for(auto a:v) total+=a;
	return total;
}

template<typename T>
double mean(std::vector<T> v){
	return sum(v)/(v.size()+0.0);
}

template<typename T>
std::vector<T> sorted(std::vector<T> v){
	sort(begin(v),end(v));
	return v;
}

std::map<std::string,std::string> env_vars(char **envp);
std::vector<std::string> args(int argc,char **argv);

template<typename T>
std::vector<T> reverse(std::set<T> s){
	std::vector<T> r1,r;
	for(auto a:s) r1|=a;
	for(int i=r1.size()-1;i>=0;i--){
		r|=r1[i];
	}
	return r;
}

template<typename T>
std::vector<T> reversed(std::vector<T> v){
	std::vector<T> r;
	for(int i=v.size()-1;i>=0;i--){
		r|=v[i];
	}
	return r;
}

int atoi(std::string const&);

#endif
