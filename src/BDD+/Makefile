CC	= g++
DIR	= ../..
INCL    = $(DIR)/include
OPT	= -O3 -I$(INCL)
OPT64  = $(OPT) -DB_64

LIB	= $(DIR)/lib/BDD.a
LIB64	= $(DIR)/lib/BDD64.a
OBJC	= $(DIR)/src/BDDc/*.o
OBJX	= $(DIR)/src/BDDXc/*.o
OBJLCM	= $(DIR)/src/BDDLCM/*.o

all:	BDD.o BDDX11.o BDDHASH.o ZBDD.o ZBDDX11.o \
	ZBDDHASH.o ZBDDLCM.o MLZBDDV.o SOP.o BtoI.o \
	CtoI.o CtoIX11.o BDDDG.o ZBDDDG.o \
	PiDD.o SeqBDD.o GraphSupport.o
	rm -f $(LIB)
	ar cr $(LIB) *.o $(OBJC) $(OBJX) $(OBJLCM)
	ranlib $(LIB)

64:     BDD_64.o BDDX11_64.o BDDHASH_64.o ZBDD_64.o ZBDDX11_64.o \
	ZBDDHASH_64.o ZBDDLCM_64.o MLZBDDV_64.o SOP_64.o BtoI_64.o \
	CtoI_64.o CtoIX11_64.o BDDDG_64.o ZBDDDG_64.o \
	PiDD_64.o SeqBDD_64.o GraphSupport_64.o
	rm -f $(LIB64)
	ar cr $(LIB64) *.o $(OBJC) $(OBJX) $(OBJLCM)
	ranlib $(LIB64)

clean:
	rm -f *.o *~

BDD.o:	BDD.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDD.cc
	rm -f BDD_64.o

BDD_64.o: BDD.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDD.cc -o BDD_64.o
	rm -f BDD.o

BDDX11.o: BDDX11.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDX11.cc
	rm -f BDDX11_64.o

BDDX11_64.o: BDDX11.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDX11.cc -o BDDX11_64.o
	rm -f BDDX11.o

BDDHASH.o: BDDHASH.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDHASH.cc
	rm -f BDDHASH_64.o

BDDHASH_64.o: BDDHASH.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDHASH.cc -o BDDHASH_64.o
	rm -f BDDHASH.o

ZBDD.o: ZBDD.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDD.cc
	rm -f ZBDD_64.o

ZBDD_64.o: ZBDD.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDD.cc -o ZBDD_64.o
	rm -f ZBDD.o

ZBDDX11.o: ZBDDX11.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDX11.cc
	rm -f ZBDDX11_64.o

ZBDDX11_64.o: ZBDDX11.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDX11.cc -o ZBDDX11_64.o
	rm -f ZBDDX11.o

ZBDDHASH.o: ZBDDHASH.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDHASH.cc
	rm -f ZBDDHASH_64.o

ZBDDHASH_64.o: ZBDDHASH.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDHASH.cc -o ZBDDHASH_64.o
	rm -f ZBDDHASH.o

ZBDDLCM.o: ZBDDLCM.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDLCM.cc
	rm -f ZBDDLCM_64.o

ZBDDLCM_64.o: ZBDDLCM.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDLCM.cc -o ZBDDLCM_64.o
	rm -f ZBDDLCM.o

MLZBDDV.o: MLZBDDV.cc $(INCL)/MLZBDDV.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c MLZBDDV.cc
	rm -f MLZBDDV_64.o

MLZBDDV_64.o: MLZBDDV.cc $(INCL)/MLZBDDV.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c MLZBDDV.cc -o MLZBDDV_64.o
	rm -f MLZBDDV.o

SOP.o:  SOP.cc $(INCL)/SOP.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c SOP.cc
	rm -f SOP_64.o

SOP_64.o: SOP.cc $(INCL)/SOP.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c SOP.cc -o SOP_64.o
	rm -f SOP.o

BtoI.o: BtoI.cc $(INCL)/BtoI.h $(INCL)/BDD.h
	$(CC) $(OPT) -c BtoI.cc
	rm -f BtoI_64.o

BtoI_64.o: BtoI.cc $(INCL)/BtoI.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c BtoI.cc -o BtoI_64.o
	rm -f BtoI.o

CtoI.o: CtoI.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c CtoI.cc
	rm -f CtoI_64.o

CtoI_64.o: CtoI.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c CtoI.cc -o CtoI_64.o
	rm -f CtoI.o

CtoIX11.o: CtoIX11.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c CtoIX11.cc
	rm -f CtoIX11_64.o

CtoIX11_64.o: CtoIX11.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c CtoIX11.cc -o CtoIX11_64.o
	rm -f CtoIX11.o

BDDDG.o: BDDDG.cc $(INCL)/BDDDG.h $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDDG.cc
	rm -f BDDDG_64.o

BDDDG_64.o: BDDDG.cc $(INCL)/BDDDG.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDDG.cc -o BDDDG_64.o
	rm -f BDDDG.o

ZBDDDG.o: ZBDDDG.cc $(INCL)/ZBDDDG.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDDG.cc
	rm -f ZBDDDG_64.o

ZBDDDG_64.o: ZBDDDG.cc $(INCL)/ZBDDDG.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDDG.cc -o ZBDDDG_64.o
	rm -f ZBDDDG.o

PiDD.o: PiDD.cc $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c PiDD.cc
	rm -f PiDD_64.o

PiDD_64.o: PiDD.cc $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c PiDD.cc -o PiDD_64.o
	rm -f PiDD.o

SeqBDD.o: SeqBDD.cc $(INCL)/SeqBDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c SeqBDD.cc
	rm -f SeqBDD_64.o

SeqBDD_64.o: SeqBDD.cc $(INCL)/SeqBDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c SeqBDD.cc -o SeqBDD_64.o
	rm -f SeqBDD.o

GraphSupport.o: GraphSupport.cc $(INCL)/GraphSupport.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c GraphSupport.cc
	rm -f GraphSupport_64.o

GraphSupport_64.o: GraphSupport.cc $(INCL)/GraphSupport.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c GraphSupport.cc -o GraphSupport_64.o
	rm -f GraphSupport.o

