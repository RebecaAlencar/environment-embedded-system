
all:
	g++ main.cpp sources/*.cpp -o a.out -I headers -lxerces-c

clean:
	rm -f a.out
	rm -f output/*
