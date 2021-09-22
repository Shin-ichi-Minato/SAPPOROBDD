/************************************************
 * PiDD class (SAPPORO-1.55) - Header           *
 * (C) Shin-ichi MINATO  (Dec. 11, 2012)        *
 ************************************************/

class PiDD;

#ifndef _PiDD_
#define _PiDD_

#include "ZBDD.h"

class PiDD;

#define PiDD_MaxVar 254

#define PiDD_Y_YUV(y, u, v) ((y==u)? v:(y==v)? u:y)
#define PiDD_U_XYU(x, y, u) ((x==u)? y:u)
 
#define PiDD_X_Lev(lev) (PiDD_XOfLev[lev])
#define PiDD_Y_Lev(lev) (PiDD_LevOfX[PiDD_XOfLev[lev]] -lev +1)
#define PiDD_Lev_XY(x,y) (PiDD_LevOfX[x] -y +1)

extern int PiDD_TopVar;
extern int PiDD_VarTableSize;
extern int PiDD_LevOfX[PiDD_MaxVar];
extern int *PiDD_XOfLev;

extern int PiDD_NewVar(void);
extern int PiDD_VarUsed(void);

extern PiDD operator*(const PiDD&, const PiDD&);
extern PiDD operator/(const PiDD&, const PiDD&);

class PiDD
{
  ZBDD _zbdd;
public:
  PiDD(void) { _zbdd = 0; }
  PiDD(int a) { _zbdd = a; }
  PiDD(const PiDD& f) { _zbdd = f._zbdd; }
  PiDD(const ZBDD& zbdd) { _zbdd = zbdd; }

  ~PiDD(void) { }

  PiDD& operator=(const PiDD& f) { _zbdd = f._zbdd; return *this; }
  PiDD& operator&=(const PiDD&); // inline
  PiDD& operator+=(const PiDD&); // inline
  PiDD& operator-=(const PiDD&); // inline
  PiDD& operator*=(const PiDD&); // inline
  PiDD& operator/=(const PiDD&); // inline
  PiDD& operator%=(const PiDD&); // inline
  PiDD Swap(int, int) const;
  PiDD Cofact(int, int) const;

  PiDD Odd(void) const;
  PiDD Even(void) const;
  PiDD SwapBound(int) const;

  int TopX(void) const { return PiDD_X_Lev(TopLev()); }
  int TopY(void) const { return PiDD_Y_Lev(TopLev()); }
  int TopLev(void) const { return BDD_LevOfVar(_zbdd.Top()); }
  bddword Size(void) const;
  bddword Card(void) const;
  ZBDD GetZBDD(void) const { return _zbdd; }
  
  void Print(void) const;
  void Enum(void) const;
  void Enum2(void) const;

  friend PiDD operator&(const PiDD&, const PiDD&);
  friend PiDD operator+(const PiDD&, const PiDD&);
  friend PiDD operator-(const PiDD&, const PiDD&);
  friend PiDD operator*(const PiDD&, const PiDD&);
  friend PiDD operator/(const PiDD&, const PiDD&);
  friend int operator==(const PiDD&, const PiDD&);
};

inline PiDD operator&(const PiDD& p, const PiDD& q)
  { return PiDD(p._zbdd & q._zbdd); }

inline PiDD operator+(const PiDD& p, const PiDD& q)
  { return PiDD(p._zbdd + q._zbdd); }

inline PiDD operator-(const PiDD& p, const PiDD& q)
  { return PiDD(p._zbdd - q._zbdd); }

inline PiDD operator%(const PiDD& f, const PiDD& p)
  { return f - (f/p) * p; }

inline int operator==(const PiDD& p, const PiDD& q)
  { return p._zbdd == q._zbdd; }

inline int operator!=(const PiDD& p, const PiDD& q)
  { return !(p == q); }

inline PiDD& PiDD::operator&=(const PiDD& f) { return *this = *this & f; }
inline PiDD& PiDD::operator+=(const PiDD& f) { return *this = *this + f; }
inline PiDD& PiDD::operator-=(const PiDD& f) { return *this = *this - f; }
inline PiDD& PiDD::operator*=(const PiDD& f) { return *this = *this * f; }
inline PiDD& PiDD::operator/=(const PiDD& f) { return *this = *this / f; }
inline PiDD& PiDD::operator%=(const PiDD& f) { return *this = *this % f; }

#endif // _PiDD_

