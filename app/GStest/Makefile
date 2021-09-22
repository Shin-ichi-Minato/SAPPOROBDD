CC	= g++

#OPT 	= -O3 
OPT = -O3 -DB_64

OBJ	= GStest.o

PROGRAM = GStest

DIR	= ../..
INCLUDE = -I$(DIR)/include 
LIBDIR	= $(DIR)/lib

all:	$(PROGRAM)    

$(PROGRAM): $(OBJ)
	$(CC) $(OPT) $(INCLUDE) \
	$(OBJ) $(LIBDIR)/BDD64.a -o $(PROGRAM)
#	$(OBJ) $(LIBDIR)/BDD.a -o $(PROGRAM)

GStest.o: GStest.cc
	$(CC) $(INCLUDE) $(OPT) -c GStest.cc

clean:
	rm -f *.o
	rm -f lex.yy.*
	rm -f y.tab.*

# DO NOT DELETE THIS LINE -- make depend depends on it.
