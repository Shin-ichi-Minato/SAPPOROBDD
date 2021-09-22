CC	= g++

DIR	= ../..
INCL    = $(DIR)/include
LIB	= $(DIR)/lib/BDD32.a
LIB64	= $(DIR)/lib/BDD64.a
XLIB64  = -lX11
#XLIB64  = /usr/X11R6/lib64/libX11.a

PRG     = isop32
PRG64   = isop
PRG_ST  = isop32-static
PRG64_ST = isop-static

OPT 	= -O3 -Wall -Wextra -Wshadow -DYY_ALWAYS_INTERACTIVE -I$(INCL)
OPT64   = $(OPT) -DB_64
OPT_ST  = $(OPT) -DB_STATIC -static
OPT64_ST = $(OPT64) -DB_STATIC -static

OBJ	= isop.o
OBJ64   = isop_64.o
OBJ_ST  = isop_st.o
OBJ64_ST = isop_64_st.o

all:	$(PRG64)    

32:	$(PRG)    

static:	$(PRG64_ST)    

32static: $(PRG_ST)    

$(PRG): $(OBJ) $(LIB)
	$(CC) $(OPT) $(OBJ) $(LIB) -lX11 -o $(PRG)

$(PRG64): $(OBJ64) $(LIB64)
	$(CC) $(OPT64) $(OBJ64) $(LIB64) $(XLIB64) -o $(PRG64)

$(PRG_ST): $(OBJ_ST) $(LIB)
	$(CC) $(OPT_ST) $(OBJ_ST) $(LIB) -o $(PRG_ST)

$(PRG64_ST): $(OBJ64_ST) $(LIB64)
	$(CC) $(OPT64_ST) $(OBJ64_ST) $(LIB64) -o $(PRG64_ST)

isop.o: isop.cc isop.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c isop.cc

isop_64.o: isop.cc isop.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c isop.cc -o isop_64.o

isop_st.o: isop.cc isop.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT_ST) -c isop.cc -o isop_st.o

isop_64_st.o: isop.cc isop.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64_ST) -c isop.cc -o isop_64_st.o

clean:
	rm -f *.o
	rm -f lex.yy.*
	rm -f y.tab.*

