.PHONY: build clean run
build:
	g++ -Wall -std=c++11 -g -o lang_detection lang_detection.cpp
clean:
	rm -f lang_detection *~
run:
	./lang_detection
