CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

OBJ = main.o functionLib.o httpserver

all: httpserver

httpserver: main.c functionLib.c
	gcc -O2 -g -o $@ $^ 

clean:
	rm -f $@ $(OBJ)
