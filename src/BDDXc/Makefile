CC    = gcc
DIR   = ../..
INCL  = $(DIR)/include
OPT   = -O3 -Wall -W -I$(INCL) -I/opt/X11/include
OPT64 = $(OPT) -DB_64

WINDOWDEF = -DLINE

all:	graph_64.o interface_64.o location_64.o parts11_64.o \
	reorder_64.o train_64.o window11_64.o

32:	graph_32.o interface_32.o location_32.o parts11_32.o \
	reorder_32.o train_32.o window11_32.o

64:	graph_64.o interface_64.o location_64.o parts11_64.o \
	reorder_64.o train_64.o window11_64.o

clean:
	rm -f *.o *.a *~

graph_32.o: graph.c train.h defs.h reorder.h $(INCL)/bddc.h
	$(CC) $(OPT) -c graph.c -o graph_32.o

graph_64.o: graph.c train.h defs.h reorder.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c graph.c -o graph_64.o

interface_32.o: interface.c $(INCL)/bddc.h
	$(CC) $(OPT) -c interface.c -o interface_32.o

interface_64.o: interface.c $(INCL)/bddc.h
	$(CC) $(OPT64) -c interface.c  -o interface_64.o

location_32.o: location.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c location.c -o location_32.o

location_64.o: location.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c location.c -o location_64.o

parts11_32.o: parts11.c defs.h $(INCL)/bddc.h
	$(CC) $(OPT) $(WINDOWDEF) -c parts11.c -o parts11_32.o

parts11_64.o: parts11.c defs.h $(INCL)/bddc.h
	$(CC) $(OPT64) $(WINDOWDEF) -c parts11.c -o parts11_64.o

reorder_32.o: reorder.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c reorder.c -o reorder_32.o

reorder_64.o: reorder.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c reorder.c -o reorder_64.o

train_32.o: train.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c train.c -o train_32.o

train_64.o: train.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c train.c -o train_64.o

window11_32.o: window11.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) $(WINDOWDEF) -c window11.c -o window11_32.o

window11_64.o: window11.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) $(WINDOWDEF) -c window11.c -o window11_64.o

