#ifndef RECORD_H
#define RECORD_H

#include<iosfwd>

struct Record{
	unsigned win,loss,tie;

	Record();
};

double win_portion(Record);//from 0 to 1
bool operator<(Record,Record);
std::ostream& operator<<(std::ostream&,Record);

#endif
