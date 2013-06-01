CFILES=record.cpp match_info.cpp main.cpp scrape.cpp exception.cpp str.cpp util.cpp team.cpp bevent.cpp

scraper: $(CFILES)
	g++ -O3 --std=c++0x -o scraper $(CFILES) -ljson_spirit

.PHONY: clean
clean:
	rm -f scraper
