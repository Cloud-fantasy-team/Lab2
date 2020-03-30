CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: try_6

try_6: main_try_6.c neighbor.cc sudoku_basic.cc sudoku_min_arity.cc sudoku_min_arity_cache.cc sudoku_dancing_links_2.cc
	g++ -O2 -o $@ $^ -pthread
clean: 
	-rm try_6
