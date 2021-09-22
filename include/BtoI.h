/******************************************
 * Binary-to-Integer function class       *
 * (SAPPORO-1.55) - Header                *
 * (C) Shin-ichi MINATO  (Dec. 11, 2012)  *
 ******************************************/

class BtoI;

#ifndef _BtoI_
#define _BtoI_

#include "BDD.h"

class BtoI
{
  BDDV _bddv;

  BtoI Shift(int) const;
  BtoI Sup(void) const;

public:
  BtoI(void) { _bddv = BDDV(0); }
  BtoI(const BtoI& fv) { _bddv = fv._bddv; }
  BtoI(const BDDV& fv) { _bddv = fv; }
  BtoI(const BDD& f)
    { _bddv = f; if(f != 0 && f != -1) _bddv = _bddv || BDDV(0); }
  BtoI(int);
  ~BtoI(void) { }

  BtoI& operator=(const BtoI& fv) { _bddv = fv._bddv; return *this; }
  BtoI& operator+=(const BtoI&);
  BtoI& operator-=(const BtoI&);
  BtoI& operator*=(const BtoI&);
  BtoI& operator/=(const BtoI&);
  BtoI& operator%=(const BtoI&);
  BtoI& operator&=(const BtoI&);
  BtoI& operator|=(const BtoI&);
  BtoI& operator^=(const BtoI&);
  BtoI& operator<<=(const BtoI&);
  BtoI& operator>>=(const BtoI&);
  
  BtoI operator-(void) const { return 0 - *this; }
  BtoI operator~(void) const { return BtoI(~_bddv); }
  BtoI operator!(void) const;
  BtoI operator<<(const BtoI&) const;
  BtoI operator>>(const BtoI& fv) const { return *this << -fv; }
  
  BtoI UpperBound(void) const;
  BtoI UpperBound(const BDD&) const;
  BtoI LowerBound(void) const { return -(- *this).UpperBound(); }
  BtoI LowerBound(const BDD& f) const { return -(- *this).UpperBound(f); }
  
  BtoI At0(int v) const {
    if(BDD_LevOfVar(v) > BDD_TopLev())
      BDDerr("BtoI::At0: Invalid VarID.", v);
    return BtoI(_bddv.At0(v)).Sup();
  }

  BtoI At1(int v) const {
    if(BDD_LevOfVar(v) > BDD_TopLev())
      BDDerr("BtoI::At1: Invalid VarID.", v);
    return BtoI(_bddv.At1(v)).Sup();
  }

  BtoI Cofact(const BtoI&) const;

  BtoI Spread(int k) const { return BtoI(_bddv.Spread(k)).Sup(); }

  int Top(void) const { return _bddv.Top(); }

  BDD GetSignBDD(void) const { return _bddv.GetBDD(Len() - 1); }

  BDD GetBDD(int i) const
    { return _bddv.GetBDD((i >= Len())? Len()-1: i); }

  BDDV GetMetaBDDV(void) const { return _bddv; }

  int Len(void) const { return _bddv.Len(); }

  int GetInt(void) const;
  int StrNum10(char *) const;
  int StrNum16(char *) const;
  bddword Size() const;

  void Print() const;

  friend int operator==(const BtoI&, const BtoI&);
  
  friend BtoI operator+(const BtoI&, const BtoI&);
  friend BtoI operator-(const BtoI&, const BtoI&);
  friend BtoI operator&(const BtoI&, const BtoI&);
  friend BtoI operator|(const BtoI&, const BtoI&);
  friend BtoI operator^(const BtoI&, const BtoI&);
  friend BtoI BtoI_ITE(const BDD&, const BtoI&, const BtoI&);
};

extern BtoI operator+(const BtoI&, const BtoI&);
extern BtoI operator-(const BtoI&, const BtoI&);
extern BtoI operator*(const BtoI&, const BtoI&);
extern BtoI operator/(const BtoI&, const BtoI&);
extern BtoI operator&(const BtoI&, const BtoI&);
extern BtoI operator|(const BtoI&, const BtoI&);
extern BtoI operator^(const BtoI&, const BtoI&);
extern BtoI BtoI_ITE(const BDD&, const BtoI&, const BtoI&);

extern BtoI BtoI_EQ(const BtoI&, const BtoI&);

extern BtoI BtoI_atoi(char *);

inline int operator==(const BtoI& a, const BtoI& b)
  { return a._bddv == b._bddv; }

inline int operator!=(const BtoI& a, const BtoI& b) { return !(a == b); }

inline BtoI operator%(const BtoI& a, const BtoI&b )
  { return a - (a / b) * b; }

inline BtoI BtoI_ITE(const BtoI& a, const BtoI& b, const BtoI& c)
  { return BtoI_ITE(~(BtoI_EQ(a, 0).GetBDD(0)), b, c); }

inline BtoI BtoI_GT(const BtoI& a, const BtoI& b)
  { return BtoI((b - a).GetSignBDD()); }

inline BtoI BtoI_LT(const BtoI& a, const BtoI& b)
  { return BtoI((a - b).GetSignBDD()); }

inline BtoI BtoI_NE(const BtoI& a, const BtoI& b)
  { return ! BtoI_EQ(a, b); }

inline BtoI BtoI_GE(const BtoI& a, const BtoI& b)
  { return ! BtoI_LT(a, b); }

inline BtoI BtoI_LE(const BtoI& a, const BtoI& b)
  { return ! BtoI_GT(a, b); }

inline BtoI BtoI::operator!(void) const { return BtoI_EQ(*this, 0); }

inline BtoI BtoI::Cofact(const BtoI& fv) const {
  BDDV a = BDDV(BtoI_NE(fv, 0).GetBDD(0), Len());
  return BtoI(_bddv.Cofact(a)).Sup();
}

#endif // _BtoI_

