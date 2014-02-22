SHELL=/bin/bash
CFLAGS=--std=c++0x
DEPS=*.h
CFILES=$(shell ls *.cpp)
OBJ=$(CFILES:cpp=o)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

scraper: $(OBJ)
	$(CXX) $(CFLAGS) -o scraper $(OBJ) -ljson_spirit

.PHONY: clean
clean:
	rm -f scraper $(OBJ)
