
### This is the normal makefile to build tests

SRC = source/server.cc
CPPVERFLAG = -std=c++17
EXTRAARGS = -mavx2 -march=native
LDFLAGS = -lpthread
WARNINGS = -Wall -Wpedantic -Wextra -Werror
CC = g++

cache_server: $(SRC)
	$(CC) -o netflix_cache_server $(SRC) -I./include -O0 -g $(CPPVERFLAG) $(LDFLAGS) $(EXTRAARGS) $(WARNINGS)

testmake: $(SRC)
	$(CC) -o test_scalar_debug $(SRC) -I./include -O0 -g -DXXH_VECTOR=0 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	$(CC) -o test_scalar_release $(SRC) -I./include -O2 -g -DXXH_VECTOR=0 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	$(CC) -o test_sse2_debug $(SRC) -I./include -O0 -g -DXXH_VECTOR=1 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	$(CC) -o test_sse2_release $(SRC) -I./include -O2 -g -DXXH_VECTOR=1 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	$(CC) -o test_avx2_debug $(SRC) -I./include -O0 -g -DXXH_VECTOR=2 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	$(CC) -o test_avx2_release $(SRC) -I./include -O2 -g -DXXH_VECTOR=2 $(CPPVERFLAG) $(EXTRAARGS) $(WARNINGS) 
	g++ -o server server.cc -O0 -g -std=c++17 -lpthread -mavx2 -march=native -Wall -Wpedantic -Wextra -Werror
	
clean:
	rm -f ./test/test_* 
