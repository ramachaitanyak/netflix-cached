
### This is the normal makefile to build tests

CPPVERFLAG = -std=c++17
EXTRAARGS = -mavx2 -march=native
LDFLAGS = -lpthread -lstdc++
WARNINGS = -Wall -Wpedantic -Wextra -Werror
CC ?= g++
BIN = ./bin

cache_server: server.o parser.o cache.o
	$(shell mkdir -p $(BIN))
	$(shell mv server.o $(BIN)/server.o)
	$(shell mv parser.o $(BIN)/parser.o)
	$(shell mv cache.o $(BIN)/cache.o)
	$(CC) -o netflix_cache_server $(BIN)/server.o $(BIN)/parser.o $(BIN)/cache.o -I./include -O0 -g $(CPPVERFLAG) $(LDFLAGS) $(EXTRAARGS) $(WARNINGS)

server.o: server.cc
	$(CC) -g -c server.cc -I./include -O0 $(CPPVERFLAG) $(LDFLAGS) $(EXTRAARGS) $(WARNINGS)

./parser.o: parser.cc
	$(CC) -g -c parser.cc -I./include -O0 $(CPPVERFLAG) $(LDFLAGS) $(EXTRAARGS) $(WARNINGS)

./cache.o: cache.cc 
	$(CC) -g -c cache.cc -I./include -O0 $(CPPVERFLAG) $(LDFLAGS) $(EXTRAARGS) $(WARNINGS)

	
clean:
	rm -Rf $(BIN) netflix_cache_server
