CC=g++
CXXFLAGS=-std=c++14 -Werror

thread_pool_test: src/thread_pool_test.cc src/thread_pool.h
	@ printf "Compiling and testing thread pool...\n"
	@ $(CC) $(CXXFLAGS) src/thread_pool_test.cc -lpthread -o thread_pool_test
	./thread_pool_test

clean:
	@ rm ./thread_pool_test