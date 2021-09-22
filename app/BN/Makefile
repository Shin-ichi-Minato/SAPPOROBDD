CC	= g++
OPT 	= -O3 -Wall -Wextra -Wshadow -DYY_ALWAYS_INTERACTIVE 
OBJ	= table.o lex.bn.o y.tab.o yywrap.o
PROGRAM = bn
YACC	= yacc -p bn
LEX	= flex -l -Pbn

all:	$(PROGRAM)    

$(PROGRAM): $(OBJ)
	$(CC) $(OPT) $(OBJ) -o $(PROGRAM)

y.tab.cc: bnyacc.y++ bnlex.l
	$(YACC) -d -v bnyacc.y++
	$(LEX) bnlex.l
	mv y.tab.c y.tab.cc
	mv lex.bn.c lex.bn.cc

lex.bn.cc: y.tab.cc

table.o: table.cc
	$(CC) $(OPT) -c table.cc

yywrap.o: yywrap.c
	gcc $(OPT) -c yywrap.c

y.tab.o: y.tab.cc y.tab.h
	$(CC) $(OPT) -c y.tab.cc

lex.bn.o: lex.bn.cc
	$(CC) $(OPT) -c lex.bn.cc

depend:
	makedepend $(SRC) -I/usr/local/CC/incl 

clean:
	rm -f $(OBJ)
	rm -f lex.bn.*
	rm -f y.tab.*

gomi:
	rm *~

print:
	ilist $(PSRC) $(HDRS) |lpr

# DO NOT DELETE THIS LINE -- make depend depends on it.
