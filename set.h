#ifndef SET_H
#define SET_H

#include<set>
#include<iostream>
#include "util.h"

template<typename T>
std::ostream& operator<<(std::ostream& o,std::set<T> const& s){
	o<<"{ ";
	for(auto& a:s) o<<a<<" ";
	return o<<"}";
}

template<typename T>
std::set<T>& operator|=(std::set<T>& a,T t){
	a.insert(t);
	return a;
}

template<typename T,typename Collection>
std::set<T>& operator|=(std::set<T>& a,Collection const& b){
	for(auto& elem:b) a.insert(elem);
	return a;
}

template<typename T>
bool contains(std::set<T> const& s,T const& t){
	return s.find(t)!=end(s);
}

template<typename T>
std::set<T> a_and_not_b(std::set<T> const& a,std::set<T> const& b){
	std::set<T> r;
	for(auto elem:a){
		if(b.find(elem)==b.end()){
			r|=elem;
		}
	}
	return r;
}

template<typename T>
std::set<T> symmetric_difference(std::set<T> const& a,std::set<T> const& b){
	std::set<T> r;
	for(auto elem:a){
		if(!contains(b,elem)) r|=elem;
	}
	for(auto elem:b){
		if(!contains(a,elem)) r|=elem;
	}
	return r;
}

/*template<typename Collection>
auto to_set(Collection const& v)->std::set<ELEMENT(v)>{
	std::set<ELEMENT(v)> r;
	for(auto e:v) r|=e;
	return r;
}*/

template<typename T>
std::set<T> to_set(std::vector<T> const& v){
	std::set<T> r;
	for(auto e:v) r|=e;
	return r;
}

template<typename T,unsigned LEN>
std::set<T> to_set(std::array<T,LEN> a){
	std::set<T> r;
	for(auto elem:a) r|=elem;
	return r;
}

template<typename T>
std::vector<T> to_vector(std::set<T> s){
	std::vector<T> r;
	for(auto a:s) r|=a;
	return r;
}

#endif
