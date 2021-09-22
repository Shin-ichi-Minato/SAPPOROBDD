/***************************************************
 * Multi-Level ZBDDV class (SAPPORO-1.00) - Header *
 * (C) Shin-ichi MINATO  (Aug 6, 2008 )            *
 ***************************************************/

class MLZBDDV;

#ifndef _MLZBDDV_
#define _MLZBDDV_

#include "ZBDD.h"

class MLZBDDV;

class MLZBDDV
{
  int _pin;
  int _out;
  int _sin;
  ZBDDV _zbddv;

public:
  MLZBDDV(void);

  MLZBDDV(ZBDDV& zbddv);
  MLZBDDV(ZBDDV& zbddv, int pin, int out);

  ~MLZBDDV(void);

  MLZBDDV& operator=(const MLZBDDV&);

  int N_pin(void);
  int N_out(void);
  int N_sin(void);
  ZBDDV GetZBDDV(void);
  
  void Print(void);
  
};

#endif // _MLZBDDV_

