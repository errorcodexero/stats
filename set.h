#ifndef SET_H
#define SET_H

template<typename T>
std::ostream& operator<<(std::ostream&,std::set<T>)nyi

template<typename T>
std::set<T>& operator|=(std::set<T>& a,T t){
	a.insert(t);
	return a;
}

#endif
