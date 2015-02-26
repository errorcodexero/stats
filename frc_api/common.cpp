#include "common.h"

using namespace std;

Positive_int::Positive_int(unsigned a):x(a){
	assert(x>0);
}

unsigned Positive_int::get()const{ return x; }

ostream& operator<<(ostream& o,Positive_int a){
	return o<<a.get();
}

Team_number::Team_number(int v):x(v){
	assert(x>0 && x<6000);
}

int Team_number::get()const{ return x; }

ostream& operator<<(ostream& o,Team_number a){
	return o<<"team"<<a.get();
}
/*
template<typename A,typename B>
class At_least_one{
	Maybe<A> a_;
	Maybe<B> b_;

	public:
	explicit At_least_one(A a):a_(a){}
	explicit At_least_one(B b):b_(b){}
	explicit At_least_one(A a,B b):a_(a),b_(b){}

	Maybe<A> a()const{ return a_; }
	Maybe<B> b()const{ return b_; }
};

template<typename A,typename B>
ostream& operator<<(ostream& o,At_least_one<A,B> const& a){
	o<<"At_least_one(";
	o<<a.a()<<a.b();
	return o<<")";
}
*/
/*
template<typename A,typename B>
class At_most_one{
	Maybe<A> a_;
	Maybe<B> b_;

	public:
	At_most_one(){}
	explicit At_most_one(A a):a_(a){}
	explicit At_most_one(B b):b_(b){}

	Maybe<A> a()const{ return a_; }
	Maybe<B> b()const{ return b_; }
};

template<typename A,typename B>
ostream& operator<<(ostream& o,At_most_one<A,B> const& a){
	o<<"At_most_one(";
	o<<a.a()<<a.b();
	return o<<")";
}
*/
