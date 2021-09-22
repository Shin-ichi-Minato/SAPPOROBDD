CC    = g++
DIR   = ../..
INCL  = $(DIR)/include
OPT   = -O3 -D_NO_MAIN_ -I$(INCL)
OPT64 = $(OPT) -DB_64

all:	lcm-vsop.o

64:	lcm-vsop_64.o

clean:
	rm -f *.o *.a *~

lcm-vsop.o: lcm-vsop.cc 
	$(CC) $(OPT) -c lcm-vsop.cc 
	rm -f lcm-vsop_64.o

lcm-vsop_64.o: lcm-vsop.cc 
	$(CC) $(OPT64) -c lcm-vsop.cc -o lcm-vsop_64.o
	rm -f lcm-vsop.o

lcm-vsop.cc: lcm.c problem.c itemset.c trsact.c base.c \
             queue.c stdlib2.c aheap.c sgraph.c vec.c lcm-vsop.sed \
	     aheap.h base.h itemset.h problem.h queue.h \
	     sgraph.h stdlib2.h trsact.h vec.h
	sed -f lcm-vsop.sed < lcm.c > lcm-vsop.cc

