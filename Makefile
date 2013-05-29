scraper: exception.cpp scrape.cpp str.cpp util.cpp
	g++ -O3 --std=c++0x -o scraper exception.cpp scrape.cpp str.cpp util.cpp -ljson_spirit

.PHONY: clean
clean:
	rm -f scraper
