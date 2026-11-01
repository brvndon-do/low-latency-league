CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

all: test

test: tests.cpp
	$(CXX) $(CXXFLAGS) -o tests tests.cpp engine.cpp
	./tests

# ---------------------------------------------------------------------------
# NOTE: submit target will not work since I don't have access to their benchmark
#		suite. use "bench" target instead.
# ---------------------------------------------------------------------------
submit: engine.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c engine.cpp -o engine.o
	$(CXX) $(CXXFLAGS) -shared -o engine.so engine.o
	lll-bench $(MAKEFILE_DIR)engine.so -d 1

bench: benchmark.cpp
	$(CXX) $(CXXFLAGS) -o benchmark benchmark.cpp engine.cpp
	./benchmark
	 
clean:
	rm -f tests engine.o engine.so
