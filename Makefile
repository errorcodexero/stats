SHELL=/bin/bash
CXX=g++
CFLAGS=--std=c++11 -Wall -Wextra
DEPS=*.h
CFILES=$(shell ls *.cpp | grep -v cmp.cpp)
OBJ=$(CFILES:cpp=o)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

scraper: $(OBJ)
	$(CXX) $(CFLAGS) -o scraper $(OBJ) -ljson_spirit

.PHONY: clean
clean:
	rm -f scraper $(OBJ)
