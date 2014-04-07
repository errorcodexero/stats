#ifndef MAP_H
#define MAP_H

#include "set.h"

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& m){
	std::vector<V> r;
	for(auto a:m) r|=a.second;
	return r;
}

template<typename K,typename V>
std::set<K> keys(std::map<K,V> const& m){
	std::set<K> r;
	for(auto p:m) r|=p.first;
	return r;
}

#endif
