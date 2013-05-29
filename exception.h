#ifndef EXCEPTION_H
#define EXCEPTION_H

#include<exception>
#include<string>

#define T File_not_found
struct T:public std::exception{
	std::string msg;

	T();
	T(T const& t);
	explicit T(std::string const& s);

	~T()throw();
	T& operator=(T const&);

	const char *what()const throw();
};

std::ostream& operator<<(std::ostream&,T const&);
#undef T

#define T Value_not_found
struct T:public std::exception{
	std::string msg;

	T();
	explicit T(std::string const&);
	T(T const&);
	T& operator=(T const&);
	~T()throw();

	const char *what()const throw();
};

std::ostream& operator<<(std::ostream&,T const&);
#undef T

#endif
