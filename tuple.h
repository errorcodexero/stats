#ifndef TUPLE_H
#define TUPLE_H

#include <iostream>
#include <tuple>

//adapted from http://en.cppreference.com/w/cpp/utility/tuple/tuple 
// helper function to print a tuple of any size
template<class Tuple, std::size_t N>
struct TuplePrinter {
	static void print(std::ostream& o,std::string const& sep,const Tuple& t){
		TuplePrinter<Tuple, N-1>::print(o,sep,t);
		o<<sep<<std::get<N-1>(t);
	}
};
 
template<class Tuple>
struct TuplePrinter<Tuple, 1>{
	static void print(std::ostream& o,std::string const&,const Tuple& t){
		o<<std::get<0>(t);
	}
};

template<class... Args>
void print(std::ostream& o,std::string const& sep,std::tuple<Args...> const& t){
	TuplePrinter<decltype(t),sizeof...(Args)>::print(o,sep,t);
}

template<class... Args>
std::ostream& operator<<(std::ostream& o,const std::tuple<Args...>& t){
	o<<"(";
	TuplePrinter<decltype(t), sizeof...(Args)>::print(o,",",t);
	return o<<")";
}

template<typename Tuple,int COL>
std::vector<Tuple> sort_tuples(std::vector<Tuple> v){
	sort(
		begin(v),end(v),
		[](Tuple a,Tuple b)->bool{
			if(std::get<COL>(a)<std::get<COL>(b)) return 1;
			if(std::get<COL>(b)<std::get<COL>(a)) return 0;
			return a<b;
		}
	);
	return v;
}

#endif
