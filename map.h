#ifndef MAP_H
#define MAP_H

//#include "vector.h"

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& m){
	std::vector<V> r;
	for(auto a:m) r|=a.second;
	return r;
}

#endif
