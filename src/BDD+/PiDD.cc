/****************************************
 * PiDD class (SAPPORO-1.59)            *
 * (Main part)                          *
 * (C) Shin-ichi MINATO (Dec. 10, 2013) *
 ****************************************/

#include <math.h>
#include "PiDD.h"

using std::cout;

//----------- Internal constant data for PiDD -----------
static const char BC_PiDD_SWAP = 80;
static const char BC_PiDD_COFACT = 81;
static const char BC_PiDD_MULT = 82;
static const char BC_PiDD_DIV = 83;
static const char BC_PiDD_ODD = 84;

//----------- Macros for operation cache -----------
#define PiDD_CACHE_CHK_RETURN(op, fx, gx) \
  { ZBDD z = BDD_CacheZBDD(op, fx, gx); \
    if(z != -1) return PiDD(z); \
    BDD_RECUR_INC; }

#define PiDD_CACHE_ENT_RETURN(op, fx, gx, h) \
  { BDD_RECUR_DEC; \
    if(h != -1) BDD_CacheEnt(op, fx, gx, h._zbdd.GetID()); \
    return h; }

//----------- External functions for PiDD ----------
int PiDD_TopVar = 0;
int PiDD_VarTableSize = 16;
int PiDD_LevOfX[PiDD_MaxVar];
int *PiDD_XOfLev;

int PiDD_NewVar()
{
   if(PiDD_TopVar == PiDD_MaxVar) 
     BDDerr("PiDD_NewVar: Too large var ", (bddword) PiDD_TopVar);

   if(PiDD_TopVar == 0)
   {
     PiDD_XOfLev = new int[PiDD_VarTableSize];
     PiDD_XOfLev[0] = 0;
     PiDD_LevOfX[0] = 0;
     PiDD_LevOfX[1] = 0;
   }

   for(int i=0; i<PiDD_TopVar; i++) BDD_NewVar();
   PiDD_TopVar++;  

   int toplev = BDD_TopLev();
   if(PiDD_TopVar > 1) PiDD_LevOfX[PiDD_TopVar] = toplev;

   if(PiDD_VarTableSize <= toplev)
   {
     int size = PiDD_VarTableSize;
     int *table = PiDD_XOfLev;
     PiDD_VarTableSize <<= 2;
     PiDD_XOfLev = new int[PiDD_VarTableSize];
     for(int i=0; i<size; i++) PiDD_XOfLev[i] = table[i];
     delete[] table;
   }

   for(int i=0; i<(PiDD_TopVar-1); i++)
     PiDD_XOfLev[toplev - i] = PiDD_TopVar;

   return PiDD_TopVar;
}

int PiDD_VarUsed() { return PiDD_TopVar; }

PiDD operator*(const PiDD& p, const PiDD& q)
{
  if(p == 0) return 0;
  if(q == 0) return 0;
  if(p == 1) return q;
  if(q == 1) return p;
  if(p == -1) return -1;
  if(q == -1) return -1;

  bddword pz = p._zbdd.GetID();
  bddword qz = q._zbdd.GetID();
  PiDD_CACHE_CHK_RETURN(BC_PiDD_MULT, pz, qz);

  int qx = q.TopX();
  int qy = q.TopY();
  int top = q._zbdd.Top();
  PiDD q0 = PiDD(q._zbdd.OffSet(top));
  PiDD q1 = PiDD(q._zbdd.OnSet0(top));

  PiDD r = (p * q0) + (p * q1).Swap(qx, qy);
  
  PiDD_CACHE_ENT_RETURN(BC_PiDD_MULT, pz, qz, r);
}

PiDD operator/(const PiDD& f, const PiDD& p)
{
  if(f == -1) return -1;
  if(p == -1) return -1;
  if(p == 1) return f;
  if(p == 0) BDDerr("operator/(): Divided by zero.");
  int fx = f.TopX(); int px = p.TopX();
  if(fx < px) return 0;

  bddword fz = f._zbdd.GetID();
  bddword pz = p._zbdd.GetID();
  PiDD_CACHE_CHK_RETURN(BC_PiDD_DIV, fz, pz);

  int py = p.TopY();
  int top = p._zbdd.Top();
  PiDD p1 = PiDD(p._zbdd.OnSet0(top));
  PiDD q = (f.Cofact(px, py) / p1).Cofact(py, py);
  if(q != 0)
  {
    PiDD p0 = PiDD(p._zbdd.OffSet(top));
    if(p0 != 0) q &= f / p0;
  }

  PiDD_CACHE_ENT_RETURN(BC_PiDD_DIV, fz, pz, q);
}

//-------------- Class methods of PiDD -----------------

/*
PiDD& PiDD::operator&=(const PiDD& f) { return *this = *this & f; }
PiDD& PiDD::operator+=(const PiDD& f) { return *this = *this + f; }
PiDD& PiDD::operator-=(const PiDD& f) { return *this = *this - f; }
PiDD& PiDD::operator*=(const PiDD& f) { return *this = *this * f; }
PiDD& PiDD::operator/=(const PiDD& f) { return *this = *this / f; }
PiDD& PiDD::operator%=(const PiDD& f) { return *this = *this % f; }
*/

PiDD PiDD::Swap(int u, int v) const
{
  if(_zbdd == -1) return -1;
  int m = PiDD_VarUsed();
  if(u <= 0 || u > m)
     BDDerr("PiDD::Swap(): Invalid U ", (bddword) u);
  if(v <= 0 || v > m)
     BDDerr("PiDD::Swap(): Invalid V ", (bddword) v);
  if(u == v) return *this;
  if(u < v) return Swap(v, u);

  int x = TopX();
  int y = TopY();

  if(x < u)
    return PiDD(_zbdd.Change(BDD_VarOfLev(PiDD_Lev_XY(u, v))));

  bddword pz = _zbdd.GetID();
  bddword qz = u * (PiDD_MaxVar + 1) + v;
  PiDD_CACHE_CHK_RETURN(BC_PiDD_SWAP, pz, qz);

  int top = _zbdd.Top();
  PiDD p0 = PiDD(_zbdd.OffSet(top));
  PiDD p1 = PiDD(_zbdd.OnSet0(top));

  PiDD r = p0.Swap(u, v)
         + p1.Swap(PiDD_U_XYU(x,y,u), v).Swap(x, PiDD_Y_YUV(y,u,v));

  PiDD_CACHE_ENT_RETURN(BC_PiDD_SWAP, pz, qz, r);
}

PiDD PiDD::Cofact(int u, int v) const
{
  if(_zbdd == -1) return -1;
  int m = PiDD_VarUsed();
  if(u <= 0 || u > m)
     BDDerr("PiDD::Cofact(): Invalid U ", (bddword) u);
  if(v <= 0 || v > m)
     BDDerr("PiDD::Cofact(): Invalid V ", (bddword) v);

  int x = TopX();
  if(x < u || x < v) return (u == v)? *this: 0;

  int y = TopY();
  if(x == u && y > v) return 0;

  int top = _zbdd.Top();
  PiDD p0 = PiDD(_zbdd.OffSet(top));
  PiDD p1 = PiDD(_zbdd.OnSet0(top));

  if(x == u) return (y == v)? p1: p0.Cofact(u, v);
  if(y == v) return p0.Cofact(u, v);
  
  bddword pz = _zbdd.GetID();
  bddword qz = u * (PiDD_MaxVar + 1) + v;
  PiDD_CACHE_CHK_RETURN(BC_PiDD_COFACT, pz, qz);

  PiDD r = p0.Cofact(u, v);
  if(u >= v) r += p1.Cofact(u, v).Swap(x, PiDD_Y_YUV(y, u, v));
  else r += p1.Cofact(u, PiDD_U_XYU(x,y,v)).Swap(x, PiDD_Y_YUV(y,v,u));

  PiDD_CACHE_ENT_RETURN(BC_PiDD_COFACT, pz, qz, r);
}

PiDD PiDD::Odd() const
{
  if(_zbdd == -1) return -1;

  int x = TopX();
  if(x == 0) return 0;

  bddword pz = _zbdd.GetID();
  PiDD_CACHE_CHK_RETURN(BC_PiDD_ODD, pz, 0);

  int y = TopY();
  int top = _zbdd.Top();
  PiDD p0 = PiDD(_zbdd.OffSet(top));
  PiDD p1 = PiDD(_zbdd.OnSet0(top));
  PiDD r = p0.Odd() + p1.Even().Swap(x,y);

  PiDD_CACHE_ENT_RETURN(BC_PiDD_ODD, pz, 0, r);
}

PiDD PiDD::Even() const { return *this - this->Odd(); }

PiDD PiDD::SwapBound(int n) const { return PiDD(_zbdd.PermitSym(n)); }

bddword PiDD::Size() const { return _zbdd.Size(); }
bddword PiDD::Card() const { return _zbdd.Card(); }

void PiDD::Print() const { _zbdd.Print(); }

static int* VarMap;
static int Flag;
static int Depth;

static void PiDD_Enum(PiDD, int);
static void PiDD_Enum(PiDD p, int dim)
{
  if(p == -1) return;
  if(p == 0) return;
  if(p == 1)
  {
    if(Flag) cout << " + ";
    else Flag = 1;
    int d0 = 0;
    for(int i=0; i<dim; i++) if(VarMap[i] != i + 1) d0 = i + 1;
    if(d0 == 0) cout << "1";
    else
    {
      cout << "[";
      for(int i=0; i<d0; i++)
      {
        if(i > 0) cout << " ";
        int a = VarMap[i];
        if(a == i + 1) cout << ".";
        else cout << a;
      }
      cout << "]";
    }
    return;
  }
  int x = p.TopX();
  int y = p.TopY();
  PiDD p1 = p.Cofact(x, y);
  PiDD p0 = p - p1.Swap(x, y);
  PiDD_Enum(p0, dim);
  int t;
  t = VarMap[x-1]; VarMap[x-1] = VarMap[y-1]; VarMap[y-1] = t;
  PiDD_Enum(p1, dim);
  t = VarMap[x-1]; VarMap[x-1] = VarMap[y-1]; VarMap[y-1] = t;
}

void PiDD::Enum() const
{
  if(*this == -1)
  {
    cout << "(undefined)\n";
    cout.flush();
    return;
  }
  if(*this == 0)
  {
    cout << "0\n";
    cout.flush();
    return;
  }
  if(*this == 1)
  {
    cout << "1\n";
    cout.flush();
    return;
  }

  Flag = 0;
  int dim = TopX();
  VarMap = new int[dim];
  for(int i=0; i<dim; i++) VarMap[i] = i+1;
  PiDD_Enum(*this, dim);
  delete[] VarMap;
  cout << "\n";
  cout.flush();
}

static void PiDD_Enum2(PiDD);
static void PiDD_Enum2(PiDD p)
{
  if(p == -1) return;
  if(p == 0) return;
  if(p == 1)
  {
    if(Flag) cout << " + ";
    else Flag = 1;
    if(Depth == 0) cout << "1";
    else
    {
      for(int i=0; i<Depth; i++)
      {
        int a = VarMap[Depth - i - 1];
	int x = PiDD_X_Lev(a);
	int y = PiDD_Y_Lev(a);
	cout << "(" << x << ":" << y << ")";
      }
    }
    return;
  }
  int x = p.TopX();
  int y = p.TopY();
  PiDD p1 = p.Cofact(x, y);
  PiDD p0 = p - p1.Swap(x, y);
  PiDD_Enum2(p0);
  VarMap[Depth++] = p.TopLev();
  PiDD_Enum2(p1);
  Depth--;
}

void PiDD::Enum2() const
{
  if(*this == -1)
  {
    cout << "(undefined)\n";
    cout.flush();
    return;
  }
  if(*this == 0)
  {
    cout << "0\n";
    cout.flush();
    return;
  }
  if(*this == 1)
  {
    cout << "1\n";
    cout.flush();
    return;
  }

  Flag = 0;
  int dim = TopX();
  VarMap = new int[dim];
  Depth = 0;
  PiDD_Enum2(*this);
  delete[] VarMap;
  cout << "\n";
  cout.flush();
}

