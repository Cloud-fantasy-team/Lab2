CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

THREADFLAGS= -lpthread
BASICFILE = main.cc neighbor.cc sudoku_basic.cc sudoku_min_arity.cc sudoku_min_arity_cache.cc sudoku_dancing_links.cc
MULTITHREAD = multi_thread.cc


all: sudoku_solve

sudoku_solve: $(BASICFILE) $(MULTITHREAD)
	g++ -O2 -o $@ $(BASICFILE) $(MULTITHREAD) $(THREADFLAGS) 

run:
	make clean -si
	make -si
	./sudoku_solve ./test5 c

clean: 

	rm sudoku_solveCXXFLAGS+=-O2 -ggdb -DDEBUG

