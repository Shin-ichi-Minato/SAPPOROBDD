/****************************************
 * PiDD class (SAPPORO-1.95)            *
 * (Main part)                          *
 * (C) Yuma INOUE (May 2, 2022)         *
 ****************************************/

#include <vector>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include "RotPiDD.h"

//----------- Internal constant data for RotPiDD -----------
static const char BC_RotPiDD_LeftRot = 100;
static const char BC_RotPiDD_Swap = 101;
static const char BC_RotPiDD_Reverse = 102;
static const char BC_RotPiDD_COFACT = 103;
static const char BC_RotPiDD_MULT = 104;
static const char BC_RotPiDD_ODD = 105;
static const char BC_RotPiDD_ORDER = 106;
static const char BC_RotPiDD_Inverse = 107;
static const char BC_RotPiDD_INSERT = 108;
static const char BC_RotPiDD_NORMALIZE = 109;
static const char BC_RotPiDD_REMOVEMAX = 110;

//----------- Macros for operation cache -----------
#define RotPiDD_CACHE_CHK_RETURN(op, fx, gx) \
  { ZBDD z = BDD_CacheZBDD(op, fx, gx); \
    if(z != -1) return RotPiDD(z); \
    BDD_RECUR_INC; }

#define RotPiDD_CACHE_ENT_RETURN(op, fx, gx, h) \
  { BDD_RECUR_DEC; \
    if(h != -1){ BDD_CacheEnt(op, fx, gx, h._zbdd.GetID());} \
    return h; }

//----------- External functions for RotPiDD ----------
int RotPiDD_TopVar = 0;
int RotPiDD_VarTableSize = 16;
int RotPiDD_LevOfX[RotPiDD_MaxVar];
int *RotPiDD_XOfLev;

int RotPiDD_NewVar()
{
   if(RotPiDD_TopVar == RotPiDD_MaxVar)
     BDDerr("PiDD_NewVar: Too large var ", (bddword) RotPiDD_TopVar);

   if(RotPiDD_TopVar == 0)
   {
     RotPiDD_XOfLev = new int[RotPiDD_VarTableSize];
     RotPiDD_XOfLev[0] = 0;
     RotPiDD_LevOfX[0] = 0;
     RotPiDD_LevOfX[1] = 0;
   }

   for(int i=0; i<RotPiDD_TopVar; i++) BDD_NewVar();
   RotPiDD_TopVar++;

   int toplev = BDD_TopLev();
   if(RotPiDD_TopVar > 1) RotPiDD_LevOfX[RotPiDD_TopVar] = toplev;

   if(RotPiDD_VarTableSize <= toplev)
   {
     int size = RotPiDD_VarTableSize;
     int *table = RotPiDD_XOfLev;
     RotPiDD_VarTableSize <<= 2;
     RotPiDD_XOfLev = new int[RotPiDD_VarTableSize];
     for(int i=0; i<size; i++) RotPiDD_XOfLev[i] = table[i];
     delete[] table;
   }

   for(int i=0; i<(RotPiDD_TopVar-1); i++)
     RotPiDD_XOfLev[toplev - i] = RotPiDD_TopVar;

   return RotPiDD_TopVar;
}

int RotPiDD_VarUsed() { return RotPiDD_TopVar; }

RotPiDD operator*(const RotPiDD& p, const RotPiDD& q)
{
  if(p == -1 || q == -1)return -1;
  if(p == 0 || q == 0) return 0;
  if(p == 1) return q;
  if(q == 1) return p;

  bddword pz = p._zbdd.GetID();
  bddword qz = q._zbdd.GetID();
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_MULT, pz, qz);

  int qx = q.TopX();
  int qy = q.TopY();
  int top = q._zbdd.Top();
  RotPiDD q0 = RotPiDD(q._zbdd.OffSet(top));
  RotPiDD q1 = RotPiDD(q._zbdd.OnSet0(top));

  RotPiDD r = (p * q0) + (p * q1).LeftRot(qx, qy);

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_MULT, pz, qz, r);
}

//-------------- Class methods of RotPiDD -----------------
RotPiDD RotPiDD::LeftRot(int u, int v) const
{
  if(_zbdd == -1) return -1;
  int m = RotPiDD_VarUsed();
  if(u <= 0 || u > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid U ", (bddword) u);
  if(v <= 0 || v > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid V ", (bddword) v);
  if(u == v) return *this;
  if(u < v) return LeftRot(v, u);

  int x = TopX();
  int y = TopY();

  if(x < u)
    return RotPiDD(_zbdd.Change(BDD_VarOfLev(RotPiDD_Lev_XY(u, v))));

  bddword pz = _zbdd.GetID();
  bddword qz = u * (RotPiDD_MaxVar + 1) + v;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_LeftRot, pz, qz);

  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));

  RotPiDD np0 = p0.LeftRot(u,v), np1 = p1;
  int ny = y;
  if(u<y){
    np1 = p1.LeftRot(u,v);
  }else if(u==y){
    ny = v;
  }else if(v<=y){
    ny = y+1; np1 = p1.LeftRot(u-1,v);
  }else{
    np1 = p1.LeftRot(u-1,v-1);
  }
  RotPiDD r = np0 + np1.LeftRot(x,ny);

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_LeftRot, pz, qz, r);
}

RotPiDD RotPiDD::Swap(int a, int b) const
{
  if(_zbdd == -1) return -1;
  int m = RotPiDD_VarUsed();
  if(a <= 0 || a > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid U ", (bddword) a);
  if(b <= 0 || b > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid V ", (bddword) b);
  if(a == b) return *this;
  if(a > b) return Swap(b, a);

  bddword pz = _zbdd.GetID();
  bddword qz = a * (RotPiDD_MaxVar + 1) + b;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_Swap, pz, qz);

  RotPiDD swap(1);
  swap = swap.LeftRot(a, b);
  for (int i = b - 1; i > a; --i) {
    swap = RotPiDD(1).LeftRot(i-1, i) * swap;
  }

  RotPiDD r = swap * *this;

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_Swap, pz, qz, r);
}

RotPiDD RotPiDD::Reverse(int l, int r) const
{
  if(_zbdd == -1) return -1;
  int m = RotPiDD_VarUsed();
  if(l <= 0 || l > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid U ", (bddword) l);
  if(r <= 0 || r > m)
     BDDerr("PiDD::Rot_PreRot(): Invalid V ", (bddword) r);
  if(l == r) return *this;
  if(l > r) return Swap(r, l);

  bddword pz = _zbdd.GetID();
  bddword qz = l * (RotPiDD_MaxVar + 1) + r;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_Reverse, pz, qz);

  RotPiDD reverse(1);
  for (int i = r; i > l; --i) {
    reverse = RotPiDD(1).LeftRot(l, i) * reverse;
  }

  RotPiDD res = reverse * *this;

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_Reverse, pz, qz, res);
}

RotPiDD RotPiDD::Cofact(int u, int v) const
{
  if(_zbdd == -1) return -1;
  int m = RotPiDD_VarUsed();
  if(u <= 0 || u > m)
     BDDerr("PiDD::Cofact(): Invalid U ", (bddword) u);
  if(v <= 0 || v > m)
     BDDerr("PiDD::Cofact(): Invalid V ", (bddword) v);

  int x = TopX();
  if(x < u || x < v) return (u == v)? *this: 0;

  int y = TopY();
  if(x == u && y > v) return 0;

  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));

  if(x == u) return (y == v)? p1: p0.Cofact(u, v);
  if(y == v) return p0.Cofact(u, v);

  bddword pz = _zbdd.GetID();
  bddword qz = u * (RotPiDD_MaxVar + 1) + v;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_COFACT, pz, qz);

  RotPiDD r = p0.Cofact(u, v);
  if(u >= v) r += p1.Cofact(u, v).LeftRot(x, RotPiDD_Y_YUV(y, u, v));
  else r += p1.Cofact(u, RotPiDD_U_XYU(x,y,v)).LeftRot(x, RotPiDD_Y_YUV(y,v,u));

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_COFACT, pz, qz, r);
}

RotPiDD RotPiDD::Odd() const
{
  if(_zbdd == -1) return -1;

  int x = TopX();
  if(x == 0) return 0;

  bddword pz = _zbdd.GetID();
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_ODD, pz, 0);

  int y = TopY();
  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));
  RotPiDD r = p0.Odd() + p1.Even().LeftRot(x,y);

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_ODD, pz, 0, r);
}

RotPiDD RotPiDD::Even() const { return *this - this->Odd(); }

RotPiDD RotPiDD::RotBound(int n) const { return RotPiDD(_zbdd.PermitSym(n)); }

RotPiDD RotPiDD::Order(int a, int b) const
{
  if(_zbdd == -1) return -1;
  if(_zbdd == 0)return 0;
  if(_zbdd == 1)return (a<b)?1:0;

  int x = TopX();
  if(x<a && b<a)return 0;
  if(x<b && a<b)return *this;

  bddword pz = _zbdd.GetID();
  bddword qz = a * (RotPiDD_MaxVar + 1) + b;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_ORDER, pz, qz);

  int y = TopY();
  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));

  RotPiDD r = p0.Order(a,b);
  if(y==b)r += p1.LeftRot(x,y);
  else if(y!=a)r += p1.Order(y<a?a-1:a, y<b?b-1:b).LeftRot(x,y);

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_ORDER, pz, qz, r);
}

RotPiDD RotPiDD::Inverse(void) const {
  if(_zbdd==-1 || _zbdd==0 || _zbdd==1)return *this;

  bddword pz = _zbdd.GetID();
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_Inverse, pz, 0);

  int x = TopX(), y = TopY();
  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));
  RotPiDD invrot = RotPiDD(1);
  for (int i = y; i < x; ++i) {
    invrot = invrot.LeftRot(i, i+1);
  }

  RotPiDD r = p0.Inverse() + invrot * p1.Inverse();

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_Inverse, pz, 0, r);
}

RotPiDD RotPiDD::Insert(int p, int v) const
{
  if(_zbdd == 0 || _zbdd == -1) return _zbdd;

  bddword pz = _zbdd.GetID();
  bddword qz = p * (RotPiDD_MaxVar + 1) + v;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_INSERT, pz, qz);

  int x = TopX();
  int y = TopY();
  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));

  RotPiDD r;
  if(_zbdd == 1 || x+1 < v) {
    if(p < v) r = this->Insert(p,v-1).LeftRot(v,v-1);
    r = this->LeftRot(p,v);
  } else if(x < p) {
    r = this->LeftRot(p,v);
  } else {
    int nx = x + (p<=x ? 1 : 0), ny = y + (v<=y ? 1 : 0);
    int np = p, nv = v - (y<v ? 1 : 0);
    r = p0.Insert(p,v) + p1.Insert(np,nv).LeftRot(nx,ny);
  }

  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_INSERT, pz, qz, r);
}

RotPiDD RotPiDD::RemoveMax(int k) const {
  if(_zbdd == 0 || _zbdd == -1 || _zbdd == 1) return _zbdd;
  int x = TopX();
  if(x < k) return *this;

  bddword pz = _zbdd.GetID();
  bddword qz = k;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_REMOVEMAX, pz, qz);

  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top)).RemoveMax(k);

  int y = TopY();
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top)).RemoveMax(k-1);

  RotPiDD r = p0 + p1.LeftRot(x-1,y);
  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_REMOVEMAX, pz, qz, r);
}

//normalize n-permutation into [1..n]
void RotPiDD::normalizePerm(std::vector<int> &v){
  std::vector<int> pos = v;
  sort(pos.begin(),pos.end());
  for(int i=0;i<(int)v.size();i++){
    v[i] = upper_bound(pos.begin(),pos.end(),v[i]) - pos.begin();
  }
}

//convert std::vector<int> [1..n] into RotPiDD in O(n) time
RotPiDD RotPiDD::VECtoRotPiDD(std::vector<int> v){
  normalizePerm(v);
  int n = v.size();

  std::vector<int> id(n);
  for(int i=0;i<n;i++) id[i] = i + 1;

  //decomposition of transposition
  std::vector<std::pair<int,int> > rots;

  for (int i = n - 1; i >= 0; --i) {
    for (int j = 0; j < i; ++j){
      if (id[j] == v[i]) {
      	rots.push_back(std::make_pair(j+1, i+1));
      	for (int k = j; k < i; ++k) {
      	  std::swap(id[k], id[k+1]);
      	}
      	break;
      }
    }
  }

  RotPiDD res(1);
  for(int i=(int)rots.size()-1;i>=0;i--){
    res = res.LeftRot(rots[i].first, rots[i].second);
  }
  return res;
}

RotPiDD RotPiDD::normalizeRotPiDD(int k) const {
  int x = TopX();
  if(x <= k) return *this;

  bddword pz = _zbdd.GetID();
  bddword qz = k;
  RotPiDD_CACHE_CHK_RETURN(BC_RotPiDD_NORMALIZE, pz, qz);

  int top = _zbdd.Top();
  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top)).normalizeRotPiDD(k);
  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top)).normalizeRotPiDD(k);

  RotPiDD r = p0 + p1;
  RotPiDD_CACHE_ENT_RETURN(BC_RotPiDD_NORMALIZE, pz, qz, r);
}

long long int RotPiDD::contradictionMaximization(unsigned long long int used_set, std::vector<int>& unused_list, int n,
						std::unordered_map< std::pair<bddword, unsigned long long int>, long long int, hash_func> &hash,
						const std::vector< std::vector<int> > &w) const {
  if(_zbdd == 0 || _zbdd == -1) return -1e9; //TODO: define INF

  std::pair<bddword, unsigned long long int> key(_zbdd.GetID(), used_set);
  if( hash.count(key) ) return hash[key];

  if(_zbdd == 1) return 0;

  int top = _zbdd.Top();
  int x = TopX(), y = TopY();
  for (int i = x; i < n; ++i) {
    used_set += (unsigned long long int)(1) << unused_list[i];
  }

  RotPiDD p0 = RotPiDD(_zbdd.OffSet(top));
  long long int left_val = p0.contradictionMaximization(used_set, unused_list, x, hash, w);

  RotPiDD p1 = RotPiDD(_zbdd.OnSet0(top));
  long long int right_val = 0;
  for (int i = y; i < x; ++i) {
    std::swap(unused_list[i - 1], unused_list[i]);
  }
  int removed_element = unused_list[x - 1];
  used_set += (bddword)(1) << removed_element;
  for(int i = y - 1; i < x - 1; ++i) {
    right_val += w[unused_list[x-1]][unused_list[i]];
  }
  right_val += p1.contradictionMaximization(used_set, unused_list, x - 1, hash, w);
  for (int i = x - 1; i >= y; --i) {
    std::swap(unused_list[i - 1], unused_list[i]);
  }

  return hash[key] = std::max(left_val, right_val);
}

bddword RotPiDD::Size() const { return _zbdd.Size(); }
bddword RotPiDD::Card() const { return _zbdd.Card(); }

void RotPiDD::Print() const { _zbdd.Print(); }

static int* VarMap;
static int Flag;
static int Depth;

RotPiDD RotPiDD::Extract_One(void){
  if(*this == 1 || *this == 0 || *this == -1)return *this;

  int top = _zbdd.Top();
  return RotPiDD(_zbdd.OnSet0(top)).Extract_One().LeftRot(TopX(),TopY());
}

static void RotPiDD_Enum(RotPiDD);
static void RotPiDD_Enum(RotPiDD p){
  if(p == 0)return;

  if(p == 1){
    std::cout << "[ ";
    for(int i=0;i<RotPiDD_TopVar-1;i++)std::cout << VarMap[i] << ", ";
    std::cout << VarMap[RotPiDD_TopVar-1] << " ]\n";
    std::cout.flush();
    return;
  }

  int x = p.TopX();
  int y = p.TopY();
  RotPiDD p1 = p.Cofact(x, y);
  RotPiDD p0 = p - p1.LeftRot(x, y);

  RotPiDD_Enum(p0);

  std::rotate(VarMap+y-1,VarMap+y,VarMap+x);
  RotPiDD_Enum(p1);
  std::rotate(VarMap+y-1,VarMap+x-1,VarMap+x);
  return;
}

void RotPiDD::Enum(void) const
{
  if(*this==0){
    std::cout << "0\n";
    std::cout.flush();
    return;
  }

  VarMap = new int[RotPiDD_TopVar];
  for(int i=0;i<RotPiDD_TopVar;i++)VarMap[i] = i+1;
  RotPiDD_Enum(*this);

  delete[] VarMap;
  std::cout.flush();
}

static void RotPiDD_ToVecOfPerms(RotPiDD, std::vector< std::vector<int> >&);
static void RotPiDD_ToVecOfPerms(RotPiDD p, std::vector< std::vector<int> >& res){
  if(p == 0)return;

  if(p == 1){
    std::vector<int> perm(RotPiDD_TopVar);
    for(int i=0;i<RotPiDD_TopVar;i++)perm[i] = VarMap[i];
    res.push_back(perm);
    return;
  }

  int x = p.TopX();
  int y = p.TopY();
  RotPiDD p1 = p.Cofact(x, y);
  RotPiDD p0 = p - p1.LeftRot(x, y);

  RotPiDD_ToVecOfPerms(p0,res);

  std::rotate(VarMap+y-1,VarMap+y,VarMap+x);
  RotPiDD_ToVecOfPerms(p1,res);
  std::rotate(VarMap+y-1,VarMap+x-1,VarMap+x);
  return;
}

std::vector< std::vector<int> > RotPiDD::RotPiDDToVectorOfPerms(void) const
{
  std::vector< std::vector<int> > res;
  if(*this==0){
    return res;
  }

  VarMap = new int[RotPiDD_TopVar];
  for(int i=0;i<RotPiDD_TopVar;i++)VarMap[i] = i+1;
  RotPiDD_ToVecOfPerms(*this, res);

  delete[] VarMap;
  return res;
}

static void RotPiDD_Enum2(RotPiDD);
static void RotPiDD_Enum2(RotPiDD p)
{
  if(p == -1) return;
  if(p == 0) return;
  if(p == 1)
  {
    if(Flag) std::cout << " + ";
    else Flag = 1;
    if(Depth == 0) std::cout << "1";
    else
    {
      for(int i=0; i<Depth; i++)
      {
        int a = VarMap[Depth - i - 1];
      	int x = RotPiDD_X_Lev(a);
      	int y = RotPiDD_Y_Lev(a);
      	std::cout << "(" << x << ":" << y << ")";
      }
    }
    return;
  }
  int x = p.TopX();
  int y = p.TopY();
  RotPiDD p1 = p.Cofact(x, y);
  RotPiDD p0 = p - p1.LeftRot(x, y);
  RotPiDD_Enum2(p0);
  VarMap[Depth++] = p.TopLev();
  RotPiDD_Enum2(p1);
  Depth--;
}

void RotPiDD::Enum2() const
{
  if(*this == -1)
  {
    std::cout << "(undefined)\n";
    std::cout.flush();
    return;
  }
  if(*this == 0)
  {
    std::cout << "0\n";
    std::cout.flush();
    return;
  }
  if(*this == 1)
  {
    std::cout << "1\n";
    std::cout.flush();
    return;
  }

  Flag = 0;
  int dim = TopX();
  VarMap = new int[dim];
  Depth = 0;
  RotPiDD_Enum2(*this);
  delete[] VarMap;
  std::cout << "\n";
  std::cout.flush();
}
