CC    = g++
DIR   = ../..
INCL  = $(DIR)/include
OPT   = -O3 -Wall -Wextra -Wshadow -D_NO_MAIN_ -I$(INCL)
OPT64 = $(OPT) -DB_64

LIB32	= $(DIR)/lib/BDD32.a
LIB64	= $(DIR)/lib/BDD64.a
OBJP32	= $(DIR)/src/BDD+/*_32.o
OBJP64	= $(DIR)/src/BDD+/*_64.o
OBJC32	= $(DIR)/src/BDDc/*_32.o
OBJC64	= $(DIR)/src/BDDc/*_64.o
OBJX32	= $(DIR)/src/BDDXc/*_32.o
OBJX64	= $(DIR)/src/BDDXc/*_64.o

all:	$(LIB64)

32:	$(LIB32)

64:	$(LIB64)

$(LIB64): lcm-vsop_64.o 
	  rm -f $(LIB64) 
	  ar cr $(LIB64) lcm-vsop_64.o $(OBJP64) $(OBJC64) $(OBJX64)
	  ranlib $(LIB64)

$(LIB32): lcm-vsop_32.o
	  rm -f $(LIB32)
	  ar cr $(LIB32) lcm-vsop_32.o $(OBJP32) $(OBJC32) $(OBJX32)
	  ranlib $(LIB32)

clean:
	rm -f *.o *.a *~

lcm-vsop_32.o: lcm-vsop.cc 
	$(CC) $(OPT) -c lcm-vsop.cc -o lcm-vsop_32.o

lcm-vsop_64.o: lcm-vsop.cc 
	$(CC) $(OPT64) -c lcm-vsop.cc -o lcm-vsop_64.o

lcm-vsop.cc: lcm.c problem.c itemset.c trsact.c base.c \
             queue.c stdlib2.c aheap.c sgraph.c vec.c lcm-vsop.sed \
	     aheap.h base.h itemset.h problem.h queue.h \
	     sgraph.h stdlib2.h trsact.h vec.h
	sed -f lcm-vsop.sed < lcm.c > lcm-vsop.cc

