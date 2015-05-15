#ifndef COMMON_H
#define COMMON_H

#include "../maybe.h"

class Positive_int{
	unsigned x;

	public:
	explicit Positive_int(unsigned);
	unsigned get()const;
};

std::ostream& operator<<(std::ostream&,Positive_int);

class Team_number{
	int x;

	public:
	explicit Team_number(int);
	int get()const;
};
std::ostream& operator<<(std::ostream&,Team_number);

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
std::ostream& operator<<(std::ostream& o,At_least_one<A,B> const&);

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
std::ostream& operator<<(std::ostream& o,At_most_one<A,B> const& a){
	o<<"At_most_one(";
	o<<a.a()<<a.b();
	return o<<")";
}

#endif
