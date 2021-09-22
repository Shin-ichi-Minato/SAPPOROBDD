/*********************************************
 * SeqBDD+ Class (SAPPORO-1.70) - Header     *
 * (C) Shin-ichi MINATO  (May 17, 2015)      *
 *********************************************/

class SeqBDD;
class SeqBDDV;

#ifndef _SeqBDD_
#define _SeqBDD_

#include "ZBDD.h"

extern SeqBDD operator*(const SeqBDD&, const SeqBDD&);
extern SeqBDD operator/(const SeqBDD&, const SeqBDD&);
extern SeqBDD operator%(const SeqBDD&, const SeqBDD&);

//extern SeqBDD SeqBDD_Import(FILE *strm = stdin);
//extern SeqBDD SeqBDD_ID(bddword);

class SeqBDD
{
  ZBDD _zbdd;
public:
  SeqBDD(void){ _zbdd = ZBDD(); }
  SeqBDD(int val) { _zbdd = ZBDD(val); }
  SeqBDD(const SeqBDD& f){ _zbdd = f._zbdd; }
  SeqBDD(const ZBDD& zbdd){ _zbdd = zbdd; }
  ~SeqBDD(void){ }

  SeqBDD& operator=(const SeqBDD& f) { _zbdd = f._zbdd; return *this; }
  SeqBDD operator&=(const SeqBDD& f)
    { _zbdd = _zbdd & f._zbdd; return *this; }

  SeqBDD operator+=(const SeqBDD& f)
    { _zbdd = _zbdd + f._zbdd; return *this; }
  SeqBDD operator-=(const SeqBDD& f)
    { _zbdd = _zbdd - f._zbdd; return *this; }

  SeqBDD operator*=(const SeqBDD&); // inline
  SeqBDD operator/=(const SeqBDD&); // inline
  SeqBDD operator%=(const SeqBDD&); // inline

  SeqBDD OffSet(int v) const;
  SeqBDD OnSet0(int) const;
  SeqBDD OnSet(int v) const
    { return OnSet0(v).Push(v); }
  SeqBDD Push(int v) const
    { return SeqBDD(ZBDD_ID(bddpush(_zbdd.GetID(), v))); }

  int Top(void) const { return _zbdd.Top(); }
  ZBDD GetZBDD(void) const { return _zbdd; }

  bddword Size(void) const;
  bddword Card(void) const;
  bddword Lit(void) const;
  bddword Len(void) const;
  void PrintSeq(void) const;
  void Export(FILE *strm = stdout) const;
  void Print(void) const;
  //void XPrint(void) const;

  friend SeqBDD operator&(const SeqBDD&, const SeqBDD&);
  friend SeqBDD operator+(const SeqBDD&, const SeqBDD&);
  friend SeqBDD operator-(const SeqBDD&, const SeqBDD&);
  friend SeqBDD operator*(const SeqBDD&, const SeqBDD&);
  friend SeqBDD operator/(const SeqBDD&, const SeqBDD&);
  friend SeqBDD operator%(const SeqBDD&, const SeqBDD&);
  friend int operator==(const SeqBDD&, const SeqBDD&);
  friend SeqBDD SeqBDD_ID(bddword);
};

inline SeqBDD operator&(const SeqBDD& f, const SeqBDD& g)
  { return SeqBDD(f._zbdd & g._zbdd); }

inline SeqBDD operator+(const SeqBDD& f, const SeqBDD& g)
  { return SeqBDD(f._zbdd + g._zbdd); }

inline SeqBDD operator-(const SeqBDD& f, const SeqBDD& g)
  { return SeqBDD(f._zbdd - g._zbdd); } 

inline SeqBDD operator%(const SeqBDD& f, const SeqBDD& p)
  { return f - p * (f/p); }

inline int operator==(const SeqBDD& f, const SeqBDD& g)
  { return f._zbdd == g._zbdd; }

inline int operator!=(const SeqBDD& f, const SeqBDD& g)
  { return !(f == g); }

inline SeqBDD SeqBDD::operator*=(const SeqBDD& f)
  { return *this = *this * f; }

inline SeqBDD SeqBDD::operator/=(const SeqBDD& f)
  { return *this = *this / f; }

inline SeqBDD SeqBDD::operator%=(const SeqBDD& f)
  { return *this = *this % f; }

#endif // _SeqBDD_
