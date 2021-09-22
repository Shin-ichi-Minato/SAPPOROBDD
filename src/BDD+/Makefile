CC	= g++
DIR	= ../..
INCL    = $(DIR)/include
OPT	= -O3 -Wall -W -I$(INCL)
OPT64  = $(OPT) -DB_64

LIB32	= $(DIR)/lib/BDD32.a
LIB64	= $(DIR)/lib/BDD64.a
OBJC32	= $(DIR)/src/BDDc/*_32.o
OBJC64	= $(DIR)/src/BDDc/*_64.o
OBJX32	= $(DIR)/src/BDDXc/*_32.o
OBJX64	= $(DIR)/src/BDDXc/*_64.o

all:    $(LIB64)

32:     $(LIB32)

64:     $(LIB64)

$(LIB64): BDD_64.o BDDX11_64.o BDDHASH_64.o ZBDD_64.o ZBDDX11_64.o \
	  ZBDDHASH_64.o MLZBDDV_64.o SOP_64.o BtoI_64.o \
	  CtoI_64.o CtoIX11_64.o BDDDG_64.o ZBDDDG_64.o \
	  PiDD_64.o SeqBDD_64.o GBase_64.o BDDCT_64.o
	  rm -f $(LIB64)
	  ar cr $(LIB64) *_64.o $(OBJC64) $(OBJX64)
	  ranlib $(LIB64)

$(LIB32): BDD_32.o BDDX11_32.o BDDHASH_32.o ZBDD_32.o ZBDDX11_32.o \
	  ZBDDHASH_32.o MLZBDDV_32.o SOP_32.o BtoI_32.o \
	  CtoI_32.o CtoIX11_32.o BDDDG_32.o ZBDDDG_32.o \
	  PiDD_32.o SeqBDD_32.o GBase_32.o BDDCT_32.o
	  rm -f $(LIB32)
	  ar cr $(LIB32) *_32.o $(OBJC32) $(OBJX32)
	  ranlib $(LIB32)

clean:
	rm -f *.o *~

BDD_32.o: BDD.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDD.cc -o BDD_32.o

BDD_64.o: BDD.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDD.cc -o BDD_64.o

BDDX11_32.o: BDDX11.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDX11.cc -o BDDX11_32.o

BDDX11_64.o: BDDX11.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDX11.cc -o BDDX11_64.o

BDDHASH_32.o: BDDHASH.cc $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDHASH.cc -o BDDHASH_32.o

BDDHASH_64.o: BDDHASH.cc $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDHASH.cc -o BDDHASH_64.o

ZBDD_32.o: ZBDD.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDD.cc -o ZBDD_32.o

ZBDD_64.o: ZBDD.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDD.cc -o ZBDD_64.o

ZBDDX11_32.o: ZBDDX11.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDX11.cc -o ZBDDX11_32.o

ZBDDX11_64.o: ZBDDX11.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDX11.cc -o ZBDDX11_64.o

ZBDDHASH_32.o: ZBDDHASH.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDHASH.cc -o ZBDDHASH_32.o

ZBDDHASH_64.o: ZBDDHASH.cc $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDHASH.cc -o ZBDDHASH_64.o

MLZBDDV_32.o: MLZBDDV.cc $(INCL)/MLZBDDV.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c MLZBDDV.cc -o MLZBDDV_32.o

MLZBDDV_64.o: MLZBDDV.cc $(INCL)/MLZBDDV.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c MLZBDDV.cc -o MLZBDDV_64.o

SOP_32.o: SOP.cc $(INCL)/SOP.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c SOP.cc -o SOP_32.o

SOP_64.o: SOP.cc $(INCL)/SOP.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c SOP.cc -o SOP_64.o

BtoI_32.o: BtoI.cc $(INCL)/BtoI.h $(INCL)/BDD.h
	$(CC) $(OPT) -c BtoI.cc -o BtoI_32.o

BtoI_64.o: BtoI.cc $(INCL)/BtoI.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c BtoI.cc -o BtoI_64.o

CtoI_32.o: CtoI.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c CtoI.cc -o CtoI_32.o

CtoI_64.o: CtoI.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c CtoI.cc -o CtoI_64.o

CtoIX11_32.o: CtoIX11.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c CtoIX11.cc -o CtoIX11_32.o

CtoIX11_64.o: CtoIX11.cc $(INCL)/CtoI.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c CtoIX11.cc -o CtoIX11_64.o

BDDDG_32.o: BDDDG.cc $(INCL)/BDDDG.h $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDDG.cc -o BDDDG_32.o

BDDDG_64.o: BDDDG.cc $(INCL)/BDDDG.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDDG.cc -o BDDDG_64.o

ZBDDDG_32.o: ZBDDDG.cc $(INCL)/ZBDDDG.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c ZBDDDG.cc -o ZBDDDG_32.o

ZBDDDG_64.o: ZBDDDG.cc $(INCL)/ZBDDDG.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c ZBDDDG.cc -o ZBDDDG_64.o

PiDD_32.o: PiDD.cc $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c PiDD.cc -o PiDD_32.o

PiDD_64.o: PiDD.cc $(INCL)/PiDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c PiDD.cc -o PiDD_64.o

SeqBDD_32.o: SeqBDD.cc $(INCL)/SeqBDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c SeqBDD.cc -o SeqBDD_32.o

SeqBDD_64.o: SeqBDD.cc $(INCL)/SeqBDD.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c SeqBDD.cc -o SeqBDD_64.o

GBase_32.o: GBase.cc $(INCL)/GBase.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c GBase.cc -o GBase_32.o

GBase_64.o: GBase.cc $(INCL)/GBase.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c GBase.cc -o GBase_64.o

BDDCT_32.o: BDDCT.cc $(INCL)/BDDCT.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT) -c BDDCT.cc -o BDDCT_32.o

BDDCT_64.o: BDDCT.cc $(INCL)/BDDCT.h $(INCL)/ZBDD.h $(INCL)/BDD.h
	$(CC) $(OPT64) -c BDDCT.cc -o BDDCT_64.o

