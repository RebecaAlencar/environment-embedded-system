
all:
	g++ main.cpp sources/*.cpp -o a.out -I headers -lxerces-c -lboost_filesystem -lboost_system -std=c++11

clean:
	rm -f a.out
	rm -rf output/*
