/******************************************
 * Combination-to-Integer function class  *
 * (SAPPORO-1.71) - Header                *
 * (C) Shin-ichi MINATO  (Dec. 15, 2015)  *
 ******************************************/

class CtoI;

#ifndef _CtoI_
#define _CtoI_

#include "ZBDD.h"


class CtoI
{
  ZBDD _zbdd;

public:
  CtoI(void) { _zbdd = 0; }
  CtoI(const CtoI& a) { _zbdd = a._zbdd; }
  CtoI(const ZBDD& f) { _zbdd = f; }
  CtoI(int);
  ~CtoI(void) { }

  CtoI& operator=(const CtoI& a) { _zbdd = a._zbdd; return *this; }
  CtoI& operator+=(const CtoI&); // inline
  CtoI& operator-=(const CtoI&); // inline
  CtoI& operator*=(const CtoI&); // inline
  CtoI& operator/=(const CtoI&); // inline
  CtoI& operator%=(const CtoI&); // inline

  int Top(void) const { return _zbdd.Top(); }
  int TopItem(void) const;
  int TopDigit(void) const;
  int IsBool(void) const { return (BDD_LevOfVar(Top()) <= BDD_TopLev()); }
  int IsConst(void) const { return TopItem() == 0; }

  CtoI AffixVar(int v) const
    { return CtoI((_zbdd.OffSet(v) + _zbdd.OnSet0(v)).Change(v)); }

  CtoI Factor0(int v) const { return CtoI(_zbdd.OffSet(v)); }
  CtoI Factor1(int v) const { return CtoI(_zbdd.OnSet0(v)); }

  CtoI FilterThen(const CtoI&) const;
  CtoI FilterElse(const CtoI&) const;

  CtoI FilterRestrict(const CtoI&) const;
  CtoI FilterPermit(const CtoI&) const;
  CtoI FilterPermitSym(int) const;
  CtoI Support(void) const
    { CtoI h = IsBool()? *this: NonZero(); return CtoI(h._zbdd.Support()); }

  CtoI NonZero(void) const;
  CtoI Digit(int) const;
  CtoI ConstTerm() const;

  CtoI EQ_Const(const CtoI&) const;
  CtoI NE_Const(const CtoI&) const;
  CtoI GT_Const(const CtoI&) const;
  CtoI GE_Const(const CtoI&) const;
  CtoI LT_Const(const CtoI&) const;
  CtoI LE_Const(const CtoI&) const;

  CtoI MaxVal(void) const;
  CtoI MinVal(void) const;

  CtoI CountTerms(void) const;
  CtoI TotalVal(void) const;
  CtoI TotalValItems(void) const;

  ZBDD GetZBDD(void) const { return _zbdd; }

  CtoI Abs(void) const; // inline
  CtoI Sign(void) const; // inline
  CtoI operator-(void) const; // inline

  CtoI TimesSysVar(int) const;
  CtoI DivBySysVar(int) const;
  CtoI ShiftDigit(int) const;

  bddword Size(void) const;

  int GetInt(void) const;

  int StrNum10(char *) const;
  int StrNum16(char *) const;

  int PutForm(void) const;
  void Print(void) const;

  void XPrint(void) const;
  void XPrint0(void) const;

  CtoI ReduceItems(const CtoI&) const;
  CtoI FreqPatA(int) const;
  CtoI FreqPatAV(int) const;
  CtoI FreqPatM(int) const;
  CtoI FreqPatC(int) const;

  CtoI FreqPatA2(int) const;

//  CtoI FilterClosed(CtoI);
//  CtoI FreqPatCV(int);

  friend int operator==(const CtoI&, const CtoI&);
  friend CtoI operator*(const CtoI&, const CtoI&);
  friend CtoI operator/(const CtoI&, const CtoI&);
  friend CtoI CtoI_Intsec(const CtoI&, const CtoI&);
  friend CtoI CtoI_Union(const CtoI&, const CtoI&);
  friend CtoI CtoI_Diff(const CtoI&, const CtoI&);
  friend CtoI CtoI_Meet(const CtoI&, const CtoI&);
};


extern CtoI operator+(const CtoI&, const CtoI&);
extern CtoI operator-(const CtoI&, const CtoI&);
extern CtoI operator*(const CtoI&, const CtoI&);
extern CtoI operator/(const CtoI&, const CtoI&);

extern CtoI CtoI_GT(const CtoI&, const CtoI&);
extern CtoI CtoI_GE(const CtoI&, const CtoI&);

extern CtoI CtoI_atoi(char *);

extern CtoI CtoI_Meet(const CtoI&, const CtoI&);

extern int CtoI_Lcm1(char *, char *, int, int);
extern CtoI CtoI_Lcm2(void);
extern int CtoI_LcmItems(void);
extern int CtoI_LcmPerm(int);
extern CtoI CtoI_LcmA(char *, char *, int);
extern CtoI CtoI_LcmC(char *, char *, int);
extern CtoI CtoI_LcmM(char *, char *, int);
extern CtoI CtoI_LcmAV(char *, char *, int);
extern CtoI CtoI_LcmCV(char *, char *, int);
extern CtoI CtoI_LcmMV(char *, char *, int);

inline int operator==(const CtoI& a, const CtoI& b) { return a._zbdd == b._zbdd; }
inline int operator!=(const CtoI& a, const CtoI& b) { return !(a == b); }
inline CtoI operator%(const CtoI& a, const CtoI& b) { return a - (a / b) * b; }

inline CtoI CtoI_Intsec(const CtoI& a, const CtoI& b)
  { return CtoI(a._zbdd & b._zbdd); }

inline CtoI CtoI_Union(const CtoI& a, const CtoI& b)
  { return CtoI(a._zbdd + b._zbdd); }

inline CtoI CtoI_Diff(const CtoI& a, const CtoI& b)
  { return CtoI(a._zbdd - b._zbdd); }

inline CtoI CtoI_ITE(const CtoI& a, const CtoI& b, const CtoI& c)
  { return CtoI_Union(b.FilterThen(a), c.FilterElse(a)); }

inline CtoI CtoI_NE(const CtoI& a, const CtoI& b)
  { return CtoI_Union(CtoI_Diff(a, b), CtoI_Diff(b, a)).NonZero(); }

inline CtoI CtoI_EQ(const CtoI& a, const CtoI& b)
  { return CtoI_Diff(CtoI_Union(a, b).NonZero(), CtoI_NE(a, b)); }

inline CtoI CtoI_LT(const CtoI& a, const CtoI& b) { return CtoI_GT(b, a); }
inline CtoI CtoI_LE(const CtoI& a, const CtoI& b) { return CtoI_GE(b, a); }

inline CtoI CtoI_Max(const CtoI& a, const CtoI& b)
  { return CtoI_ITE(CtoI_GT(a, b), a, b); }
inline CtoI CtoI_Min(const CtoI& a, const CtoI& b)
  { return CtoI_ITE(CtoI_GT(a, b), b, a); }

inline CtoI CtoI_Null(void) { return CtoI(ZBDD(-1)); }

inline CtoI CtoI::Abs(void) const
  { CtoI a = CtoI_GT(*this, 0); return CtoI_Union(FilterThen(a), -FilterElse(a)); }

inline CtoI CtoI::Sign(void) const
  { CtoI a = CtoI_GT(*this, 0); return a - CtoI_Diff(NonZero(), a); }

inline CtoI& CtoI::operator+=(const CtoI& a) { return *this = *this + a; }
inline CtoI& CtoI::operator-=(const CtoI& a) { return *this = *this - a; }
inline CtoI& CtoI::operator*=(const CtoI& a) { return *this = *this * a; }
inline CtoI& CtoI::operator/=(const CtoI& a) { return *this = *this / a; }
inline CtoI& CtoI::operator%=(const CtoI& a) { return *this = *this % a; }

inline CtoI CtoI::operator-(void) const { return 0 - *this; }

#endif // _CtoI_

