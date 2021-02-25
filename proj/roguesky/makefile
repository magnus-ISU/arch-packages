CC=g++
CLIBS=-lraylib -lpthread
CCFLAGS=-Wall -Wpedantic -Wextra -O3 -g

CFILES:=$(wildcard src/*.cpp)
HFILES:=$(wildcard src/*.h)
OFILES:=$(patsubst src/%, build/%, $(patsubst %.cpp, %.o, $(CFILES)))

all: roguesky

roguesky: $(OFILES)
	$(CC) $(CCFLAGS) -o $@ $(OFILES) $(CLIBS)

build/%.o: src/%.cpp $(HFILES)
	$(CC) $(CCFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf build/* roguesky
