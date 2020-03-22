
### This is the normal makefile to build cache server

CPPVERFLAG = -std=c++17
LDFLAGS = -lstdc++
WARNINGS = -Wall -Wpedantic -Wextra -Werror
CC ?= g++

cache_client: cache_client.cc
	$(CC) cache_client.cc -o test_cache_client -g $(CPPVERFLAG) $(LDFLAGS) $(WARNINGS)

clean:
	rm -Rf test_cache_client
