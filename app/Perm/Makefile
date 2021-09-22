CC	= g++
YACC	= yacc
LEX	= flex -l

DIR	= ../..
INCL    = $(DIR)/include
LIB	= $(DIR)/lib/BDD.a
LIB64	= $(DIR)/lib/BDD64.a
#XLIB64  = -lX11
XLIB64  = /usr/X11R6/lib64/libX11.a

PRG     = perm
PRG64   = perm64
PRG_ST  = perm-static
PRG64_ST = perm64-static

OPT 	= -O2 -DYY_ALWAYS_INTERACTIVE -I$(INCL)
OPT64   = $(OPT) -DB_64
OPT_ST  = $(OPT) -DB_STATIC -static
OPT64_ST = $(OPT64) -DB_STATIC -static

OBJ	= print.o table.o lex.yy.o y.tab.o yywrap.o
OBJ64   = print_64.o table_64.o lex.yy_64.o y.tab_64.o yywrap_64.o
OBJ_ST  = print_st.o table.o lex.yy.o y.tab_st.o yywrap.o
OBJ64_ST = print_64_st.o table_64.o lex.yy_64.o y.tab_64_st.o yywrap_64.o

all:	$(PRG)    

64:	$(PRG64)    

static:	$(PRG_ST)    

64static: $(PRG64_ST)    

$(PRG): $(OBJ) $(LIB)
	$(CC) $(OPT) $(OBJ) $(LIB) -lX11 -o $(PRG)

$(PRG64): $(OBJ64) $(LIB64)
	$(CC) $(OPT64) $(OBJ64) $(LIB64) $(XLIB64) -o $(PRG64)

$(PRG_ST): $(OBJ_ST) $(LIB)
	$(CC) $(OPT_ST) $(OBJ_ST) $(LIB) -o $(PRG_ST)

$(PRG64_ST): $(OBJ64_ST) $(LIB64)
	$(CC) $(OPT64_ST) $(OBJ64_ST) $(LIB64) -o $(PRG64_ST)

print.o: print.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c print.cc

print_64.o: print.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c print.cc -o print_64.o

print_st.o: print.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT_ST) -c print.cc -o print_st.o

print_64_st.o: print.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64_ST) -c print.cc -o print_64_st.o

table.o: table.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c table.cc

table_64.o: table.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c table.cc -o table_64.o

lex.yy.o: lex.yy.cc y.tab.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c lex.yy.cc

lex.yy_64.o: lex.yy.cc y.tab.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c lex.yy.cc -o lex.yy_64.o

lex.yy.cc: PermLex.l
	$(LEX) PermLex.l
	mv lex.yy.c lex.yy.cc

y.tab.o: y.tab.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c y.tab.cc

y.tab_64.o: y.tab.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c y.tab.cc -o y.tab_64.o

y.tab_st.o: y.tab.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT_ST) -c y.tab.cc -o y.tab_st.o

y.tab_64_st.o: y.tab.cc perm.h $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64_ST) -c y.tab.cc -o y.tab_64_st.o

y.tab.h: PermYacc.y++
	$(YACC) -d PermYacc.y++
	mv y.tab.c y.tab.cc

yywrap.o: yywrap.c
	gcc $(OPT) -c yywrap.c

yywrap_64.o: yywrap.c
	gcc $(OPT64) -c yywrap.c -o yywrap_64.o

clean:
	rm -f *.o
	rm -f lex.yy.*
	rm -f y.tab.*

