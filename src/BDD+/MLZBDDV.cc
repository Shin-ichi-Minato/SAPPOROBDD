/*****************************************
 * Multi-Level ZBDDV class (SAPPORO-1.59)*
 * (Main part)                           *
 * (C) Shin-ichi MINATO (Dec. 10, 2013)  *
 *****************************************/

#include "MLZBDDV.h"

using std::cout;
using std::cerr;

//-------------- Class methods of MLZBDDV -----------------

MLZBDDV::MLZBDDV()
{
  _pin = 0;
  _out = 0;
  _sin = 0;
  _zbddv = ZBDDV();
}

MLZBDDV::~MLZBDDV() { }

int MLZBDDV::N_pin() { return _pin; }
int MLZBDDV::N_out() { return _out; }
int MLZBDDV::N_sin() { return _sin; }

MLZBDDV& MLZBDDV::operator=(const MLZBDDV& v)
{
  _pin = v._pin;
  _out = v._out;
  _sin = v._sin;
  _zbddv = v._zbddv;
  return *this;
}

MLZBDDV::MLZBDDV(ZBDDV& zbddv)
{
  int pin = BDD_LevOfVar(zbddv.Top());
  int out = zbddv.Last()+1;
  MLZBDDV v = MLZBDDV(zbddv, pin, out);
  _pin = v._pin;
  _out = v._out;
  _sin = v._sin;
  _zbddv = v._zbddv;
}

MLZBDDV::MLZBDDV(ZBDDV& zbddv, int pin, int out)
{
  if(zbddv == ZBDDV(-1))
  {
    _pin = 0;
    _out = 0;
    _sin = 0;
    _zbddv = zbddv;
    return;
  }

  _pin = pin;
  _out = out;
  _sin = 0;
  _zbddv = zbddv;

  /* check each output as a divisor */
  for(int i=0; i<_out; i++)
  {
    _sin++;
    int plev = _pin + _sin;
    if(plev > BDD_TopLev()) BDD_NewVar();
    ZBDD p = _zbddv.GetZBDD(i);
    int pt = BDD_LevOfVar(p.Top());
    if(p != 0)
    {
      for(int j=0; j<_out; j++)
      {
        if(i != j)
        {
	  ZBDD f = _zbddv.GetZBDD(j);
          int ft = BDD_LevOfVar(f.Top());
	  if(ft >= pt)
	  {
	    ZBDD q = f / p;
	    if(q != 0)
	    {
	      int v = BDD_VarOfLev(plev);
	      _zbddv -= ZBDDV(f, j);
	      f = q.Change(v) + (f % p);
	      if(f == -1) { cerr << "overflow.\n"; exit(1);}
	      _zbddv += ZBDDV(f, j);
	    }
	  }
        }
      }
    }
  }

  /* extract 0-level kernels */
  for(int i=0; i<_out; i++)
  {
    ZBDD f = _zbddv.GetZBDD(i);
    while(1)
    {
      ZBDD p = f.Divisor();
      int pt = BDD_LevOfVar(p.Top());
      if(p.Top() == 0) break;
      if(p == f) break;
      _sin++;
      cout << _sin << " "; cout.flush();
      int plev = _pin + _sin;
      if(plev > BDD_TopLev()) BDD_NewVar();
      _zbddv += ZBDDV(p, _sin-1);
      int v = BDD_VarOfLev(plev);
      ZBDD q = f / p;
      _zbddv -= ZBDDV(f, i);
      f = q.Change(v) + (f % p);
      if(f == -1) { cerr << "overflow.\n"; exit(1);}
      _zbddv += ZBDDV(f, i);
      for(int j=0; j<_out; j++)
      {
        if(i != j)
        {
	  ZBDD f = _zbddv.GetZBDD(j);
          int ft = BDD_LevOfVar(f.Top());
	  if(ft >= pt)
	  {
	    ZBDD q = f / p;
	    if(q != 0)
	    {
	      _zbddv -= ZBDDV(f, j);
	      f = q.Change(v) + (f % p);
              if(f == -1) { cerr << "overflow.\n"; exit(1);}
	      _zbddv += ZBDDV(f, j);
	    }
	  }
        }
      }
    }
  }
}

void MLZBDDV::Print()
{
  cout << "pin:" << _pin << "\n";
  cout << "out:" << _out << "\n";
  cout << "sin:" << _sin << "\n";
  _zbddv.Print();
}

ZBDDV MLZBDDV::GetZBDDV()
{
  return _zbddv;
}

