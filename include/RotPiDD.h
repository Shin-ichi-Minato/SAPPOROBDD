/************************************************
 * RotPiDD class (SAPPORO-1.95) - Header        *
 * (C) Yuma INOUE  (May 2, 2022)                *
 ************************************************/

class RotPiDD;

#ifndef _RotPiDD_
#define _RotPiDD_

#include<vector>
#include<unordered_map>
#include "ZBDD.h"

class RotPiDD;

#define RotPiDD_MaxVar 254

#define RotPiDD_Y_YUV(y, u, v) ((y==u)? v:(y==v)? u:y)
#define RotPiDD_U_XYU(x, y, u) ((x==u)? y:u)

#define RotPiDD_X_Lev(lev) (RotPiDD_XOfLev[lev])
#define RotPiDD_Y_Lev(lev) (RotPiDD_LevOfX[RotPiDD_XOfLev[lev]] -lev +1)
#define RotPiDD_Lev_XY(x,y) (RotPiDD_LevOfX[x] -y +1)

extern int RotPiDD_TopVar;
extern int RotPiDD_VarTableSize;
extern int RotPiDD_LevOfX[RotPiDD_MaxVar];
extern int *RotPiDD_XOfLev;

extern int RotPiDD_NewVar(void);
extern int RotPiDD_VarUsed(void);

extern RotPiDD operator*(const RotPiDD&, const RotPiDD&);

class RotPiDD
{
  ZBDD _zbdd;
public:
  RotPiDD(void) { _zbdd = 0; }
  RotPiDD(int a) { _zbdd = a; }
  RotPiDD(const RotPiDD& f) { _zbdd = f._zbdd; }
  RotPiDD(const ZBDD& zbdd) { _zbdd = zbdd; }

  ~RotPiDD(void) { }

  RotPiDD& operator=(const RotPiDD& f) { _zbdd = f._zbdd; return *this; }
  RotPiDD& operator&=(const RotPiDD&); // inline
  RotPiDD& operator+=(const RotPiDD&); // inline
  RotPiDD& operator-=(const RotPiDD&); // inline
  RotPiDD& operator*=(const RotPiDD&); // inline
  RotPiDD LeftRot(int, int) const;
  RotPiDD Swap(int, int) const;
  RotPiDD Reverse(int, int) const;
  RotPiDD Cofact(int, int) const;

  RotPiDD Odd(void) const;
  RotPiDD Even(void) const;
  RotPiDD RotBound(int) const;
  RotPiDD Order(int, int) const;
  RotPiDD Inverse(void) const;
  RotPiDD Insert(int, int) const;
  RotPiDD RemoveMax(int) const;
  RotPiDD normalizeRotPiDD(int) const;

  static void normalizePerm(std::vector<int> &v);
  static RotPiDD VECtoRotPiDD(std::vector<int> v);

  struct hash_func{
    size_t operator()(const std::pair<bddword, unsigned long long int> &a) const {
      return (a.first + 1) * (a.second + 1);
    }
  };
  long long int contradictionMaximization(unsigned long long int, std::vector<int>&, int,
					  std::unordered_map< std::pair<bddword, unsigned long long int>, long long int, hash_func>&,
					  const std::vector< std::vector<int> >&) const;

  int TopX(void) const { return RotPiDD_X_Lev(TopLev()); }
  int TopY(void) const { return RotPiDD_Y_Lev(TopLev()); }
  int TopLev(void) const { return BDD_LevOfVar(_zbdd.Top()); }
  bddword Size(void) const;
  bddword Card(void) const;
  ZBDD GetZBDD(void) const { return _zbdd; }

  RotPiDD Extract_One(void);
  void Print(void) const;
  void Enum(void) const;
  void Enum2(void) const;
  std::vector< std::vector<int> > RotPiDDToVectorOfPerms(void) const;

  friend RotPiDD operator&(const RotPiDD&, const RotPiDD&);
  friend RotPiDD operator+(const RotPiDD&, const RotPiDD&);
  friend RotPiDD operator-(const RotPiDD&, const RotPiDD&);
  friend RotPiDD operator*(const RotPiDD&, const RotPiDD&);
  friend int operator==(const RotPiDD&, const RotPiDD&);
};

inline RotPiDD operator&(const RotPiDD& p, const RotPiDD& q)
  { return RotPiDD(p._zbdd & q._zbdd); }

inline RotPiDD operator+(const RotPiDD& p, const RotPiDD& q)
  { return RotPiDD(p._zbdd + q._zbdd); }

inline RotPiDD operator-(const RotPiDD& p, const RotPiDD& q)
  { return RotPiDD(p._zbdd - q._zbdd); }

inline int operator==(const RotPiDD& p, const RotPiDD& q)
  { return p._zbdd == q._zbdd; }

inline int operator!=(const RotPiDD& p, const RotPiDD& q)
  { return !(p == q); }

inline RotPiDD& RotPiDD::operator&=(const RotPiDD& f) { return *this = *this & f; }
inline RotPiDD& RotPiDD::operator+=(const RotPiDD& f) { return *this = *this + f; }
inline RotPiDD& RotPiDD::operator-=(const RotPiDD& f) { return *this = *this - f; }
inline RotPiDD& RotPiDD::operator*=(const RotPiDD& f) { return *this = *this * f; }

#endif // _RotPiDD_
