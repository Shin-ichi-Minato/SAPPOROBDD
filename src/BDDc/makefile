CC    = gcc
DIR   = ../..
INCL  = $(DIR)/include
OPT   = -O3 -I$(INCL)
OPT64 = $(OPT) -DB_64

all: bddc.o 

64: bddc_64.o

bddc.o: bddc.c $(INCL)/bddc.h
	$(CC) $(OPT) -c bddc.c 
	rm -f bddc_64.o

bddc_64.o: bddc.c $(INCL)/bddc.h
	$(CC) $(OPT64) -c bddc.c -o bddc_64.o
	rm -f bddc.o

clean:
	rm -f *.o *.a *~

