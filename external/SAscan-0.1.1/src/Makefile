SHELL = /bin/sh

CC = g++
CFLAGS = -Wall -Wextra -pedantic -Wshadow -static -funroll-loops -DNDEBUG -O3 -I${HOME}/include/ -L${HOME}/lib/ -Wno-unused-parameter
#CFLAGS = -Wall -Wextra -pedantic -Wshadow -g2

all: sascan

sascan: main.cpp utils.cpp
	$(CC) $(CFLAGS) -o sascan utils.cpp main.cpp -ldivsufsort -ldivsufsort64

clean:
	/bin/rm -f *.o

nuclear:
	/bin/rm -f sascan *.o gap.* sparseSA.* gt_head new_gt_head gt_tail new_gt_tail
