/************************************************
 * ZBDD-based SOP class (SAPPORO-1.55) - Header *
 * (C) Shin-ichi MINATO  (Dec. 11, 2012)        *
 ************************************************/

class SOP;
class SOPV;

#ifndef _SOP_
#define _SOP_

#include "ZBDD.h"

class SOP;
extern int SOP_NewVar(void);
extern int SOP_NewVarOfLev(int);

extern SOP operator*(const SOP&, const SOP&);
extern SOP operator/(const SOP&, const SOP&);
extern SOP operator%(const SOP&, const SOP&);

extern SOP SOP_ISOP(BDD);
extern SOP SOP_ISOP(BDD, BDD);

class SOP
{
  ZBDD _zbdd;
public:
  SOP() { _zbdd = ZBDD(); }
  SOP(int val) { _zbdd = ZBDD(val); }
  SOP(const SOP& f) { _zbdd = f._zbdd; }
  SOP(const ZBDD& zbdd) { _zbdd = zbdd; }
  ~SOP() { }
  
  SOP& operator=(const SOP& f) { _zbdd = f._zbdd; return *this; }

  SOP& operator&=(const SOP& f)
    { _zbdd = _zbdd & f._zbdd; return *this; }

  SOP& operator+=(const SOP& f)
    { _zbdd = _zbdd + f._zbdd; return *this; }

  SOP& operator-=(const SOP& f)
    { _zbdd = _zbdd - f._zbdd; return *this; }

  SOP& operator*=(const SOP&); // inline
  SOP& operator/=(const SOP&); // inline
  SOP& operator%=(const SOP&); // inline
  SOP& operator<<=(int); // inline
  SOP& operator>>=(int); // inline

  SOP operator<<(int) const;
  SOP operator>>(int) const;
  SOP And0(int) const;
  SOP And1(int) const;
  SOP Factor0(int) const;
  SOP Factor1(int) const;
  SOP FactorD(int) const;

  int Top(void) const { return (_zbdd.Top() + 1) & ~1; }

  bddword Size(void) const;
  bddword Cube(void) const;
  bddword Lit(void) const;
  
  int IsPolyCube(void) const;
  int IsPolyLit(void) const;
  SOP Divisor(void) const;
  SOP Implicants(BDD) const;
  SOP Support(void) const;
  //SOP BoolDiv(SOP, int) const;
  //SOP BoolDiv(SOP, SOP, int) const;
  //SOP BoolDiv(BDD, int) const;

  void Print(void) const;
  int PrintPla(void) const;
  
  ZBDD GetZBDD(void) const { return _zbdd; }
  BDD GetBDD(void) const;
  SOP InvISOP(void) const;

  SOP Swap(int, int) const;

  friend SOP operator&(const SOP&, const SOP&);
  friend SOP operator+(const SOP&, const SOP&);
  friend SOP operator-(const SOP&, const SOP&);
  friend int operator==(const SOP&, const SOP&);
};

inline SOP operator&(const SOP& f, const SOP& g)
  { return SOP(f._zbdd & g._zbdd); }

inline SOP operator+(const SOP& f, const SOP& g)
  { return SOP(f._zbdd + g._zbdd); }

inline SOP operator-(const SOP& f, const SOP& g)
  { return SOP(f._zbdd - g._zbdd); }

inline int operator==(const SOP& f, const SOP& g)
  { return f._zbdd == g._zbdd; }

inline int operator!=(const SOP& f, const SOP& g)
  { return !(f == g); }

inline SOP operator%(const SOP& f, const SOP& p)
  { return f - (f/p) * p; }

inline SOP& SOP::operator*=(const SOP& f)
  { return *this = *this * f; }

inline SOP& SOP::operator/=(const SOP& f)
  { return *this = *this / f; }

inline SOP& SOP::operator%=(const SOP& f)
  { return *this = *this - (*this/f) * f; }

inline SOP& SOP::operator<<=(int n) { return *this = *this << n; }
inline SOP& SOP::operator>>=(int n) { return *this = *this >> n; }




class SOPV;
extern int SOPV_NewVar(void);
extern int SOPV_NewVarOfLev(int);

extern SOPV SOPV_ISOP(BDDV);
extern SOPV SOPV_ISOP(BDDV, BDDV);
extern SOPV SOPV_ISOP2(BDDV);
extern SOPV SOPV_ISOP2(BDDV, BDDV);

class SOPV
{
  ZBDDV _v;

public:
  SOPV(void) { _v = ZBDDV(); }
  SOPV(const SOPV& v) { _v = v._v; }
  SOPV(const ZBDDV& zbddv) { _v = zbddv; }
  SOPV(const SOP& f, int loc = 0) { _v = ZBDDV(f.GetZBDD(), loc); }
  ~SOPV() { }

  SOPV& operator=(const SOPV& v) { _v = v._v; return *this; }
  SOPV operator&=(const SOPV& v) { _v = _v & v._v; return *this; }
  SOPV operator+=(const SOPV& v) { _v = _v + v._v; return *this; }
  SOPV operator-=(const SOPV& v) { _v = _v - v._v; return *this; }

  SOPV operator<<=(int); // inline
  SOPV operator>>=(int); // inline

  SOPV operator<<(int n) const { return SOPV(_v << (n<<1)); }
  SOPV operator>>(int n) const { return SOPV(_v >> (n<<1)); }

  SOPV And0(int) const;
  SOPV And1(int) const;
  SOPV Factor0(int) const;
  SOPV Factor1(int) const;
  SOPV FactorD(int) const;
  
  int Top(void) const { return (_v.Top() + 1) & ~1; }
  bddword Size(void) const;
  bddword Cube(void) const;
  bddword Lit(void) const;
  void Print(void) const;
  int PrintPla(void) const;
  
  int Last(void) const {return _v.Last(); }
  SOPV Mask(int start, int length = 1) const
    { return SOPV(_v.Mask(start, length)); }

  SOP GetSOP(int) const;
  ZBDDV GetZBDDV(void) const { return _v; }
  
  SOPV Swap(int, int) const;

  friend SOPV operator&(const SOPV&, const SOPV&);
  friend SOPV operator+(const SOPV&, const SOPV&);
  friend SOPV operator-(const SOPV&, const SOPV&);
  
  friend int operator==(const SOPV&, const SOPV&);
};

inline SOPV operator&(const SOPV& f, const SOPV& g)
  { return SOPV(f._v & g._v); }

inline SOPV operator+(const SOPV& f, const SOPV& g)
  { return SOPV(f._v + g._v); }

inline SOPV operator-(const SOPV& f, const SOPV& g)
  { return SOPV(f._v - g._v); }

inline int operator==(const SOPV& v1, const SOPV& v2)
  { return v1._v == v2._v; }

inline int operator!=(const SOPV& v1, const SOPV& v2)
  { return !(v1 == v2); }

#endif // _SOP_

