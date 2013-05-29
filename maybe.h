#ifndef MAYBE_H
#define MAYBE_H

#include<cassert>

template<typename T>
class Maybe{
	T *p;

	public:
	Maybe():p(NULL){}

	Maybe(T const& t):p(new T(t)){}

	Maybe(Maybe const& m):p(NULL){
		if(m.p){
			p=new T(*m.p);
		}
	}

	~Maybe(){
		delete p;
	}

	Maybe& operator=(Maybe const& a){
		T *next=NULL;
		if(a.p) next=new T(*a.p);
		delete p;
		p=next;
		return *this;
	}

	operator bool()const{ return !!p; }

	T& operator*(){
		assert(p);
		return *p;
	}

	T const& operator*()const{
		assert(p);
		return *p;
	}

	const T *operator->()const{
		assert(p);
		return p;
	}

	T *operator->(){
		assert(p);
		return p;
	}

	bool operator==(Maybe const& m)const{
		if(p && m){
			return *p==*m;
		}
		return !p && !m;
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& o,Maybe<T> const& m){
	if(m){
		o<<*m;
	}else{
		o<<"NULL";
	}
	return o;
}

#endif
