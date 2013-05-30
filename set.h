#ifndef SET_H
#define SET_H

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

#endif
