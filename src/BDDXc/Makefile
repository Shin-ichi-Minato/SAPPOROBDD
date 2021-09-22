CC    = gcc
DIR   = ../..
INCL  = $(DIR)/include
OPT   = -O3 -I$(INCL) -I/opt/X11/include
OPT64 = $(OPT) -DB_64

WINDOWDEF = -DLINE
#WINDOWDEF = -DSPLINE -DVERBOSE

all:	graph.o interface.o location.o parts11.o \
	reorder.o train.o window11.o

64:	graph_64.o interface_64.o location_64.o parts11_64.o \
	reorder_64.o train_64.o window11_64.o

clean:
	rm -f *.o *.a *~

graph.o: graph.c train.h defs.h reorder.h $(INCL)/bddc.h
	$(CC) $(OPT) -c graph.c 
	rm -f graph_64.o

graph_64.o: graph.c train.h defs.h reorder.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c graph.c -o graph_64.o
	rm -f graph.o

interface.o: interface.c $(INCL)/bddc.h
	$(CC) $(OPT) -c interface.c 
	rm -f interface_64.o

interface_64.o: interface.c $(INCL)/bddc.h
	$(CC) $(OPT64) -c interface.c  -o interface_64.o
	rm -f interface.o

location.o: location.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c location.c 
	rm -f location_64.o

location_64.o: location.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c location.c -o location_64.o
	rm -f location.o

parts11.o: parts11.c defs.h $(INCL)/bddc.h
	$(CC) $(OPT) $(WINDOWDEF) -c parts11.c 
	rm -f parts11_64.o

parts11_64.o: parts11.c defs.h $(INCL)/bddc.h
	$(CC) $(OPT64) $(WINDOWDEF) -c parts11.c -o parts11_64.o
	rm -f parts11.o

reorder.o: reorder.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c reorder.c 
	rm -f reorder_64.o

reorder_64.o: reorder.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c reorder.c -o reorder_64.o
	rm -f reorder.o

train.o: train.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) -c train.c 
	rm -f train_64.o

train_64.o: train.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) -c train.c -o train_64.o
	rm -f train.o

window11.o: window11.c train.h $(INCL)/bddc.h
	$(CC) $(OPT) $(WINDOWDEF) -c window11.c 
	rm -f window11_64.o

window11_64.o: window11.c train.h $(INCL)/bddc.h
	$(CC) $(OPT64) $(WINDOWDEF) -c window11.c -o window11_64.o
	rm -f window11.o

