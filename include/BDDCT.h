/*****************************************
 *  BDD Cost Table class - Header v1.87  *
 *  (C) Shin-ichi MINATO (May 14, 2021)  *
 *****************************************/

class BDDCT;

#ifndef _BDDCT_
#define _BDDCT_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include "ZBDD.h"

typedef int bddcost;
#define bddcost_null 0x7FFFFFFF
#define CT_STRLEN 15

class BDDCT
{
public:
  struct CacheEntry
  {
    bddword _id;
    bddcost _b;
    unsigned char _op;
    CacheEntry(void)
    {
      _id = BDD(-1).GetID();
      _b = bddcost_null;
      _op = 255;
    }
    ~CacheEntry(void) { }
  };

  int _n;
  bddcost *_cost;
  char **_label;
  bddcost *_submin;
  bddcost *_submax;
  bddword _casize;
  bddword _caent;
  CacheEntry* _ca;

  BDDCT(void);
  ~BDDCT(void);
  inline int Size(void) const { return _n; }
  bddcost Cost(const int ix) const;
  inline bddcost CostOfLev(const int lev) const 
  { return Cost(_n-lev); }
  char* Label(const int) const;
  inline char* LabelOfLev(const int lev) const 
  { return Label(_n-lev); }
  int Alloc(const int n, const bddcost cost = 1);
  int Import(FILE* fp = stdin);
  int AllocRand(const int, const bddcost, const bddcost);
  int SetCost(const int, const bddcost);
  inline int SetCostOfLev(const int lev, const bddcost cost) 
  { return SetCost(_n-lev, cost); }
  int SetLabel(const int, const char *);
  inline int SetLabelOfLev(const int lev, const char* label)
  { return SetLabel(_n-lev, label); }
  void Export(void) const;
  bddcost CacheRef(const unsigned char, const bddword) const;
  int CacheEnt(const unsigned char, const bddword, const bddcost);
  int CacheClear(void);
  int CacheEnlarge(void);
  ZBDD ZBDD_CostLE(const ZBDD &, const bddcost);
  ZBDD ZBDD_CostLE2(const ZBDD &, const bddcost);
  bddcost MinCost(const ZBDD &);
  bddcost MaxCost(const ZBDD &);
  bddcost MinCost2(const ZBDD &);
};

#endif // _BDDCT_

