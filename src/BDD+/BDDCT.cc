/****************************************
 * BDD Cost Table class - Body v1.87    *
 * (C) Shin-ichi MINATO (May 14, 2021)  *
 ****************************************/

#include "BDDCT.h"
using namespace std;

BDDCT::BDDCT()
{
  _n = 0;
  _cost = 0;
  _label = 0;
  _submin = 0;
  _submax = 0;
  _casize = 0;
  _caent = 0;
  _ca = 0;
}

BDDCT::~BDDCT()
{
  if(_cost) delete[] _cost; 
  if(_label)
  {
    for(int i=0; i<_n; i++) if(_label[i]) delete[] _label[i];
    delete[] _label; _label = 0;
  }
  if(_ca) delete[] _ca; 
}

bddcost BDDCT::Cost(const int ix) const
{
  return (ix >= _n)? bddcost_null: (ix < 0)? 1: _cost[ix];
}

char* BDDCT::Label(const int ix) const
{
  return (ix >= _n || ix < 0)? 0: _label[ix];
}

int BDDCT::Alloc(const int n, const bddcost cost)
{
  if(_cost) { delete[] _cost; _cost = 0; }
  if(_label)
  {
    for(int i=0; i<_n; i++) if(_label[i]) delete[] _label[i];
    delete[] _label; _label = 0;
  }

  _n = (n < 0)? 0: n;

  if(_n > 0)
  {
    if(!(_cost = new bddcost[_n])) { Alloc(0); return 1; }
    if(!(_label = new char*[_n])) { Alloc(0); return 1; }
    if(!(_submin = new bddcost[_n])) { Alloc(0); return 1; }
    if(!(_submax = new bddcost[_n])) { Alloc(0); return 1; }
    for(int i=0; i<_n; i++)
    {
      _cost[i] = cost;
      _label[i] = 0;
    }
    for(int i=0; i<_n; i++)
      if((_label[i] = new char[CT_STRLEN + 1])) _label[i][0] = 0;
      else { Alloc(0); return 1; }
  }

  if(CacheClear()) return 1;
  return 0;
}

int BDDCT::Import(FILE *fp)
{
  char s[256];
  do if(fscanf(fp, "%s", s) == EOF) return 1;
  while(s[0] == '#'); // go next word
  int n = strtol(s, NULL, 10);
  if(Alloc(n)) return 1;

  do if(fscanf(fp, "%s", s) == EOF) return 1;
  while(s[0] == '#'); // go next word
  int e = 0;
  for(int ix=0; ix<_n; ix++)
  {
    if((e = SetCost(ix, strtol(s, NULL, 10)))) break;
    if(fscanf(fp, "%s", s) == EOF) { if(ix<_n-1) e = 1; break; }
    if(s[0] == '#') 
    {
      if((e = SetLabel(ix, s+1))) break;
      do if(fscanf(fp, "%s", s) == EOF) { if(ix<_n-1) e = 1; break; }
      while(s[0] == '#'); // go next word
    }
  }
  if(e) { Alloc(0); return 1; }
  return 0;
}

int BDDCT::AllocRand(const int n, const bddcost min, const bddcost max)
{
  Alloc(n);
  bddcost m = max - min + 1;
  if(m < 0) m = 1;
  for(int ix=0; ix<_n; ix++)
    if(SetCost(ix, ((double)random()/((double)RAND_MAX+1)) * m + min))
    {
      Alloc(0);
      return 1;
    }
  return 0;
}

int BDDCT::SetCost(const int ix, const bddcost cost)
{
  if(ix < 0 || ix >= _n) return 1;
  _cost[ix] = cost;
  if(_caent > 0) if(CacheClear()) return 1;
  return 0;
}

int BDDCT::SetLabel(const int ix, const char* label)
{
  if(ix < 0 || ix >= _n) return 1;
  int j;
  for(j=0; j<CT_STRLEN; j++)
  {
    _label[ix][j] = label[j];
    if(label[j] == 0) break;
  }
  if(j == CT_STRLEN) _label[ix][j] = 0;
  return 0;
}

void BDDCT::Export() const
{
  cout << "#n " << _n << "\n";
  for(int i=0; i<_n; i++)
  {
    cout << "#" << i+1 << ": " << _cost[i];
    if(_label[i] != 0 && _label[i][0] != 0)
      cout << " #" << _label[i];
    cout << "\n";
  }
}

int BDDCT::CacheClear()
{
  if(_ca) { delete[] _ca; _ca = 0; }
  _casize = 1 << 4;
  _caent = 0;
  if(!(_ca = new CacheEntry[_casize])) return 1;
  return 0;
}

#define Hash(op, id) (id*1234567+op)

bddcost BDDCT::CacheRef(const unsigned char op, const bddword id) const
{
  if(_casize == 0) return bddcost_null;
  bddword k = Hash(op, id) & (_casize - 1);
  while(1)
  {
    if(_ca[k]._b == bddcost_null) return bddcost_null;
    if(_ca[k]._op == op && _ca[k]._id == id) return _ca[k]._b;
    k++;
    k &= _casize - 1;
  }
}

int BDDCT::CacheEnlarge()
{
  bddword newsize = _casize << 2;
  //cout << "enlarge: " << newsize << "\n";
  CacheEntry* newca = 0;
  if(!(newca = new CacheEntry[newsize])) return 1;
  for(bddword i=0; i<_casize; i++)
  {
    if(_ca[i]._b != bddcost_null)
    {
      bddword id = _ca[i]._id;
      unsigned char op = _ca[i]._op;
      bddword k = Hash(op, id) & (newsize - 1);
      while(1)
      {
        if(newca[k]._b == bddcost_null) break;
	k++;
	k &= newsize - 1;
      }
      newca[k]._op = op;
      newca[k]._id = id;
      newca[k]._b = _ca[i]._b;
    }
  }
  delete[] _ca;
  _ca = newca;
  _casize = newsize;
  return 0;
}

int BDDCT::CacheEnt(const unsigned char op, const bddword id, const bddcost b)
{
  if(_casize == 0) return 1;
  if(_caent >= (_casize >> 1) && CacheEnlarge()) return 1;
  bddword k = Hash(op, id) & (_casize - 1);
  while(1)
  {
    if(_ca[k]._b == bddcost_null) { _caent++; break; }
    if(_ca[k]._op == op && _ca[k]._id == id) break;
    k++;
    k &= _casize - 1;
  }
  _ca[k]._op = op;
  _ca[k]._id = id;
  _ca[k]._b = b;
  return 0;
}

static int N;
static BDDCT* CT;
static bddcost B;

static ZBDD CLE2(const ZBDD &, const bddcost);
ZBDD CLE2(const ZBDD& f, const bddcost spent)
{
  if(f == 0) return 0;
  if(f == 1) return (B >= spent)? 1: 0;
  bddcost b0 = CT->CacheRef(0, f.GetID());
  if(b0 != bddcost_null) if(B <= b0 + spent) return 0;
  b0 = CT->CacheRef(1, f.GetID());
  if(b0 != bddcost_null) if(B >= b0 + spent) return f;
  int top = f.Top();
  int tlev = BDD_LevOfVar(top);
  if(B < spent + CT->_submin[N-tlev]) return 0;
  if(B >= spent + CT->_submax[N-tlev]) return f;
  ZBDD h = CLE2(f.OffSet(top), spent);
  bddcost cost = CT->CostOfLev(tlev);
  h += CLE2(f.OnSet0(top), spent + cost).Change(top);
  if(h == 0) CT->CacheEnt(0, f.GetID(), B - spent);
  if(h == f) CT->CacheEnt(1, f.GetID(), B - spent);
  return h;
}

static void SetSubMinMax(void);
void SetSubMinMax()
{
  bddcost cost = CT->CostOfLev(1);
  CT->_submin[N-1] = (cost>0)? 0: cost;
  CT->_submax[N-1] = (cost>0)? cost: 0;
  for(int lev=2; lev<=N; lev++)
  {
    bddcost cost = CT->CostOfLev(lev);
    CT->_submin[N-lev] = CT->_submin[N-lev+1] + ((cost>0)? 0: cost);
    CT->_submax[N-lev] = CT->_submax[N-lev+1] + ((cost>0)? cost: 0);
  }
}

ZBDD BDDCT::ZBDD_CostLE2(const ZBDD& f, const bddcost bound)
{
  N = _n;
  CT = this;
  B = bound;
  SetSubMinMax();
  ZBDD h = CLE2(f, 0);
  return h;
}

static bddcost MinC(const ZBDD&);
bddcost MinC(const ZBDD& f)
{
  if(f == 0) return bddcost_null;
  if(f == 1) return 0;
  bddcost min = CT->CacheRef(4, f.GetID());
  if(min != bddcost_null) return min;
  int top = f.Top();
  min = MinC(f.OffSet(top));
  bddcost min1 = MinC(f.OnSet0(top))
               + CT->CostOfLev(BDD_LevOfVar(top));
  min = (min != bddcost_null && min < min1)? min: min1;
  CT->CacheEnt(4, f.GetID(), min);
  return min;
}

bddcost BDDCT::MinCost(const ZBDD& f)
{
  CT = this;
  return MinC(f);
}

static void MinC2(const ZBDD&, const bddcost);
void MinC2(const ZBDD& f, const bddcost spent)
{
  if(f == 0) return;
  if(f == 1) { B = spent - 1; return; }
  bddcost b0 = CT->CacheRef(0, f.GetID());
  if(b0 != bddcost_null) if(B <= b0 + spent) return;
  int top = f.Top();
  MinC2(f.OffSet(top), spent);
  bddcost cost = CT->CostOfLev(BDD_LevOfVar(top));
  MinC2(f.OnSet0(top), spent + cost);
  CT->CacheEnt(0, f.GetID(), B - spent);
  return;
}

bddcost BDDCT::MinCost2(const ZBDD& f)
{
  CT = this;
  if(f == 0) return bddcost_null;
  if(f == 1) return 0;
  bddcost max = 0;
  for(int ix=0; ix<_n; ix++)
  {
    bddcost cost = Cost(ix);
    if(cost > 0) max += cost;
  }
  B = max - 1;
  MinC2(f, 0);
  return B + 1;
}

static bddcost MaxC(const ZBDD&);
bddcost MaxC(const ZBDD& f)
{
  if(f == 0) return bddcost_null;
  if(f == 1) return 0;
  bddcost max = CT->CacheRef(5, f.GetID());
  if(max != bddcost_null) return max;
  int top = f.Top();
  max = MaxC(f.OffSet(top));
  bddcost max1 = MaxC(f.OnSet0(top))
               + CT->CostOfLev(BDD_LevOfVar(top));
  max = (max != bddcost_null && max > max1)? max: max1;
  CT->CacheEnt(5, f.GetID(), max);
  return max;
}

bddcost BDDCT::MaxCost(const ZBDD& f)
{
  CT = this;
  return MaxC(f);
}

/*
static ZBDD CLE(const ZBDD &, const bddcost);
ZBDD CLE(const ZBDD& f, const bddcost spent)
{
  if(f == 0) return 0;
  if(f == 1) return (B >= spent)? 1: 0;
  bddcost b0 = CT->CacheRef(4, f.GetID());
  if(b0 != bddcost_null) if(B < b0 + spent) return 0;
  b0 = CT->CacheRef(5, f.GetID());
  if(b0 != bddcost_null) if(B >= b0 + spent) return f;
  int top = f.Top();
  int tlev = BDD_LevOfVar(top);
  ZBDD h = CLE(f.OffSet(top), spent);
  bddcost cost = CT->CostOfLev(tlev);
  h += CLE(f.OnSet0(top), spent + cost).Change(top);
  return h;
}
*/

static bddcost RetMin;
static bddcost RetMax;
static ZBDD CLE(const ZBDD &, const bddcost);
ZBDD CLE(const ZBDD& f, const bddcost spent)
{
  if(f == 0)
  {
    RetMin = bddcost_null; RetMax = bddcost_null;
    return 0;
  }
  if(f == 1)
  {
    RetMin = 0; RetMax = 0;
    return (B >= spent)? 1: 0;
  }
  bddcost min = CT->CacheRef(4, f.GetID());
  bddcost max = CT->CacheRef(5, f.GetID());
  RetMin = min; RetMax = max;
  if(min != bddcost_null) if(B < min + spent) return 0;
  if(max != bddcost_null) if(B >= max + spent) return f;
  int top = f.Top();
  int tlev = BDD_LevOfVar(top);
  ZBDD h = CLE(f.OffSet(top), spent);
  int min0 = RetMin;
  int max0 = RetMax;
  bddcost cost = CT->CostOfLev(tlev);
  h += CLE(f.OnSet0(top), spent + cost).Change(top);
  if(min == bddcost_null)
  {
    min = RetMin + cost;
    if(min0 != bddcost_null) min = (min0 <= min)? min0: min;
    CT->CacheEnt(4, f.GetID(), min);
  }
  if(max == bddcost_null)
  {
    max = RetMax + cost;
    if(max0 != bddcost_null) max = (max0 >= max)? max0: max;
    CT->CacheEnt(5, f.GetID(), max);
  }
  RetMin = min; RetMax = max;
  return h;
}

ZBDD BDDCT::ZBDD_CostLE(const ZBDD& f, const bddcost bound)
{
  N = _n;
  CT = this;
  B = bound;
  ZBDD h = CLE(f, 0);
  return h;
}

