#include "bevent.h"
#include<iostream>

using namespace std;

ostream& operator<<(ostream& o,BEvent const& b){
	return o<<"BEvent("<<b.name<<" "<<b.key<<")";
}
