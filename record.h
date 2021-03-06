#ifndef RECORD_H
#define RECORD_H

#include<iosfwd>

//could be called 'team match result'
enum class Match_result{WIN,LOSS,TIE};
std::ostream& operator<<(std::ostream&,Match_result);

struct Record{
	unsigned win,loss,tie;

	Record();
	Record(unsigned,unsigned,unsigned);

	Record& operator+=(Record);
};

bool operator==(Record,Record);
bool operator<(Record,Record);
std::ostream& operator<<(std::ostream&,Record);
unsigned sum(Record);
double win_portion(Record);//from 0 to 1
Record reversed(Record);

#endif
