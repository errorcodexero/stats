#ifndef DEFAULT_H
#define DEFAULT_H

#include<iostream>

template<typename T,long long VALUE> //If the second argument is of type 'T' then the class doesn't work with floating point types.  
class Default{
	T t;

	public:
	Default():t(VALUE){}
	Default(Default const& d):t(d.t){}
	Default(T const& a):t(a){}

	template<typename INIT>
	explicit Default(INIT const& i):t(i){}

	Default& operator=(Default const& d){
		t=d.t;
		return *this;
	}

	Default& operator=(T const& v){
		t=v;
		return *this;
	}

	T& value(){
		return t;
	}

	T const& value()const{
		return t;
	}

	operator T&(){ return t; }
	operator T const&()const{ return t; }
	bool operator<(T v)const{ return t<v; }
};

template<typename T,T VALUE>
std::ostream& operator<<(std::ostream& o,Default<T,VALUE> const& d){
	return o<<d.value();
}

#endif
