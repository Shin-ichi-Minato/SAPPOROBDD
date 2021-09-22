/****************************************
 * GraphSopport class (SAPPORO-1.59)    *
 * (Main part)                          *
 * (C) Shin-ichi MINATO (Feb. 26, 2017) *
 ****************************************/

#include "GraphSupport.h"

using std::cout;
using std::cerr;

//-------------- Class methods of GraphSupport -----------------

GraphSupport::GraphSupport()
{
  _n = 0;
  _m = 0;
  _v = 0;
  _e = 0;
  _hamilton = 0;
  _f = -1; // for ZDD-constrained enumeration
}

GraphSupport::~GraphSupport()
{
  if(_v) delete[] _v;
  if(_e) delete[] _e;
}

void GraphSupport::FixEdge(const GS_e ix, const char fix)
{ _e[ix]._preset = fix; }

void GraphSupport::SetHamilton(const int hm) { _hamilton = hm; }

void GraphSupport::SetCond(ZBDD f) { _f = f; }
  // for ZDD-constrained enumeration

GraphSupport::Edge::Edge()
{
  _ev[0] = 0;
  _ev[1] = 0;
  _tmp = 0;
  _io[0] = 0;
  _io[1] = 0;
  _preset = 0;
  _mtwid = 0;
  _casize = 0;
  _caent = 0;

  _map = 0;
  _cfg = 0;
  _ca = 0;
  _ca_mate = 0;

  _f = -1; // for ZDD-constrained enumeration
}

GraphSupport::Edge::~Edge()
{
  if(_map) delete[] _map;
  if(_cfg) delete[] _cfg;
  if(_ca) delete[] _ca;
  if(_ca_mate) delete[] _ca_mate;
}

int GraphSupport::Init(const int n, const int m)
{
  _n = (n < 0)? 0: n; 
  _m = (m < 0)? 0: m;
  if(_v) { delete[] _v; _v = 0; }
  if(_e) { delete[] _e; _e = 0; }
  if(_n > 0)
    if(!(_v = new Vertex[_n])) return 1;
  if(_m > 0)
    if(!(_e = new Edge[_m])) return 1;
  return 0;
}

int GraphSupport::Pack()
{
  int i;
  for(i=0; i<_n; i++) _v[i]._tmp = 0;
  GS_v x;
  GS_v nx = 0;
  for(i=0; i<_m; i++)
  {
    for(int j=0; j<2; j++)
    {
      x = _e[i]._ev[j] - 1;
      if(_v[x]._tmp == 0) _v[x]._tmp = ++nx;
      _e[i]._ev[j] = _v[x]._tmp;
    }
  }
  if(nx < _n)
  {
    delete[] _v; _v = 0;
    _n = nx;
    if(_n > 0)
      if(!(_v = new Vertex[_n])) return 1;
  }
  return 0;
}

int GraphSupport::Import(FILE *strm)
{
  char s[256];

  do if(fscanf(strm, "%s", &s) == EOF) return 1;
  while(s[0] == '#');
  int n = strtol(s, NULL, 10);

  do if(fscanf(strm, "%s", &s) == EOF) return 1;
  while(s[0] == '#');
  int m = strtol(s, NULL, 10);

  if(Init(n, m)) return 1;

  GS_v v;
  GS_e e = 0;
  for(int i=0; i<m; i++)
  {
    for(int j=0; j<2; j++)
    {
      do if(fscanf(strm, "%s", &s) == EOF) { e = 1; break; }
      while(s[0] == '#');
      v = strtol(s, NULL, 10);
      if(v <= 0 || v > n) { e = 1; break; }
      _e[i]._ev[j] = v;
    }
  }

  if(e) { Init(0, 0); return 1; }
  return 0;
}

int GraphSupport::SetGrid(const int x, const int y)
{
  if(x < 0||y < 0) return 1;
  if(Init((x+1)*(y+1), 2*x*y+x+y)) return 1;
  GS_e p = 0;
  for(int i=0; i<y; i++)
  {
    for(int j=0; j<x; j++)
    {
      for(int k=0; k<2; k++) _e[p]._ev[k] = (x+1)*(i+k) + j+1;
      p++;
      for(int k=0; k<2; k++) _e[p]._ev[k] = (x+1)*i + j+k+1;
      p++;
    }
    for(int k=0; k<2; k++) _e[p]._ev[k] = (x+1)*(i+k) + x+1;
    p++;
  }
  for(int j=0; j<x; j++)
  {
    for(int k=0; k<2; k++) _e[p]._ev[k] = (x+1)*y + j+k+1;
    p++;
  }
  return 0;
}

void GraphSupport::Print() const
{
  cout << "#n " << (int) _n << "\n";
  cout << "#m " << (int) _m << "\n";
  for(int i=0; i<_m; i++)
    cout << (int) _e[i]._ev[0] << " " << (int) _e[i]._ev[1] << "\n";
  cout.flush();
}

inline int GraphSupport::BDDvarOfEdge(const GS_e ix) const
{ 
  while(BDD_TopLev() < _m) BDD_NewVar();
  return BDD_VarOfLev(_m - ix);
}

inline GS_e GraphSupport::EdgeOfBDDvar(const int var) const
{
  while(BDD_TopLev() < _m) BDD_NewVar();
  return _m - BDD_LevOfVar(var);
}

static GraphSupport *G;
static GraphSupport::Vertex *_v;
static GraphSupport::Edge *_e;
static GS_v _n;
static GS_e _m;

static const int HashStart = 1<<2;
static const int HashLimit = 40000000;
static const int HashTry = 1<<4;
static int _magic;

const char GS_fix0 = 1;
const char GS_fix1 = 2;

#ifdef DEBUG
static int _ent_a;
static int _ref;
static int _col;
static int _hit;
static int _count;
static int _sol;
#endif

static int CacheEnlarge(GS_e);
int CacheEnlarge(GS_e ix)
{
  int newsize = _e[ix]._casize << 1;

  GraphSupport::CacheEntry* newca = 0;
  if(!(newca = new GraphSupport::CacheEntry[newsize])) return 1;

  GS_v* newca_mate = 0;
  if(!(newca_mate =  new GS_v[_e[ix]._mtwid*newsize]))
    { delete[] newca; return 1; }

  for(int i=0; i<_e[ix]._mtwid*newsize; i++) newca_mate[i] = 0;
  for(int j=0; j<_e[ix]._casize; j++)
  {
    if(_e[ix]._ca[j]._h != -1)
    {
      int k = 0;
      bddword id = _e[ix]._ca[j]._f.GetID(); // ZDD-constrained enum.
      k = (id+(id>>10)+(id>>20)); // ZDD-constrained enum.
      for(int i=0; i<_e[ix]._mtwid; i++)
      {
        int x = _e[ix]._ca_mate[_e[ix]._mtwid*j + i];
        k ^= (i<<2)^(k<<12)^x^(x<<(2*i+3))^(x<<((11*i+7)&15)); // 8bit-v;
      }
      k &= newsize - 1;
      int k0 = k;
      int t;
      for(t=0; t<HashTry; t++)
      {
        if(newca[k]._h == -1) break; 
	k++; 
	k &= newsize - 1;
      }
      if(t == HashTry) 
        k = (k0 + (_magic++ & (HashTry-1))) & (newsize-1);
      for(int i=0; i<_e[ix]._mtwid; i++)
        newca_mate[_e[ix]._mtwid*k + i] = _e[ix]._ca_mate[_e[ix]._mtwid*j + i];
      newca[k]._f = _e[ix]._ca[j]._f; // ZDD-constrained enum.
      newca[k]._h = _e[ix]._ca[j]._h;
    }
  }
  delete[] _e[ix]._ca;
  _e[ix]._ca = newca;
  delete[] _e[ix]._ca_mate;
  _e[ix]._ca_mate = newca_mate;
  _e[ix]._casize = newsize;
  return 0;
}

static int Hash(const GS_e);
int Hash(const GS_e ix)
{
  int k = 0;
  bddword id = _e[ix]._f.GetID(); // ZDD-constrained enum.
  k = (id+(id>>10)+(id>>20)); // ZDD-constrained enum.
  for(int i=0; i<_e[ix]._mtwid; i++)
  {
    int x = _e[ix]._cfg[_e[ix]._map[i]-1];
    k ^= (i<<2)^(k<<12)^x^(x<<(2*i+3))^(x<<((11*i+7)&15)); // 8bit-v;
  }
  k &= _e[ix]._casize - 1;
  return k;
}

static ZBDD CacheCheck(const GS_e);
ZBDD CacheCheck(const GS_e ix)
{
  if(_e[ix]._casize == 0) return -1;

#ifdef DEBUG
  _ref++;
#endif
  int k = Hash(ix);
  for(int t=0; t<HashTry; t++)
  {
    if(_e[ix]._ca[k]._h == -1) return -1;
    int i;
    for(i=0; i<_e[ix]._mtwid; i++)
    {
      if(_e[ix]._ca_mate[_e[ix]._mtwid*k + i] != _e[ix]._cfg[_e[ix]._map[i]-1]) break;
    }
    if(i == _e[ix]._mtwid)
    {
      if(_e[ix]._ca[k]._f == _e[ix]._f) // ZDD-constrained enum.
      {
#ifdef DEBUG
        _hit++;
#endif
        return _e[ix]._ca[k]._h;
      }
    }
    k++;
    k &= _e[ix]._casize - 1;
  }
#ifdef DEBUG
  _col++;
#endif
  return -1;
}

static void CacheEnter(const GS_e, ZBDD);
void CacheEnter(const GS_e ix, ZBDD h)
{
  if(_e[ix]._casize == 0) return;

  if(_e[ix]._caent++ > _e[ix]._casize &&
     _e[ix]._casize < (HashLimit>>1))
  {
    if(CacheEnlarge(ix)) cerr << "enlarge failed.\n";
  }

#ifdef DEBUG
  _ent_a++;
#endif
  int k = Hash(ix);
  int k0 = k;
  int t;
  for(t=0; t<HashTry; t++)
  {
    if(_e[ix]._ca[k]._h == -1) break;
    k++; 
    k &= _e[ix]._casize - 1;
  }
  if(t == HashTry) 
    k = (k0 + (_magic++ & (HashTry-1))) & (_e[ix]._casize-1);
  for(int i=0; i<_e[ix]._mtwid; i++)
    _e[ix]._ca_mate[_e[ix]._mtwid*k + i] = _e[ix]._cfg[_e[ix]._map[i]-1];
  _e[ix]._ca[k]._f = _e[ix]._f; // ZDD-constrained enum.
  _e[ix]._ca[k]._h = h;
}

static int EnumCyclesInit(void);
int EnumCyclesInit()
{
  _v = G -> _v;
  _e = G -> _e;
  _n = G -> _n;
  _m = G -> _m;

  // new BDDvar
  while(BDD_TopLev() < _m) BDD_NewVar();

  // init degree 
  for(int i=0; i<_n; i++) { _v[i]._deg = 0; _v[i]._tmp = 0; }

  // count degrees of vartexes and init aux vars of edges
  for(int i=0; i<_m; i++)
  {
    for(int j=0; j<2; j++)
    {
      _v[_e[i]._ev[j] - 1]._deg++;
      _e[i]._io[j] = 0;
    }
    _e[i]._mtwid = 0;
  }

  // count _mtwid[ix] and set _io[ix], _maxwid, _lastin
  G->_maxwid = 0;
  G->_lastin = 0;
  for(int i=0; i<_m; i++)
  {
    if(i > 0) _e[i]._mtwid = _e[i-1]._mtwid;
    for(int p=0; p<2; p++)
    {
      int v = _e[i]._ev[p];
      if(_v[v-1]._tmp == 0)
      {
        _e[i]._io[p] |= 1;
	_e[i]._mtwid++;
	G->_lastin = (G->_lastin < i)? i: G->_lastin;
      }
      if(++_v[v-1]._tmp == _v[v-1]._deg)
      {
        _e[i]._io[p] |= 2;
	_e[i]._mtwid--;
      }
    }
    G->_maxwid = (G->_maxwid < _e[i]._mtwid)? _e[i]._mtwid: G->_maxwid;
  }

  for(int i=0; i<_m; i++)
  {
    // malloc and init _map

    if(_e[i]._map) { delete[] _e[i]._map; _e[i]._map = 0; }
    if(!(_e[i]._map = new GS_v[G->_maxwid])) return 1;

    for(int j=0; j<G->_maxwid; j++) _e[i]._map[j] = 0;

    // set _map
    int k = 0;
    if(i > 0)
    {
      for(int j=0; j<_e[i-1]._mtwid; j++)
      {
        int v = _e[i-1]._map[j];
	int is_out = 0;
	for(int p=0; p<2; p++)
	  if((_e[i]._io[p] & 2) != 0 && _e[i]._ev[p] == v)
	    is_out = 1; 
	if(!is_out) _e[i]._map[k++] = v;
      }
    }
    for(int j=0; j<2; j++)
      for(int p=0; p<2; p++)
	if((_e[i]._io[p] & 1) != 0)
	  _e[i]._map[k++] = _e[i]._ev[p];
  }

  // init MateCache
  for(int i=0; i<_m; i++)
  {
    _e[i]._casize = HashStart;
    _e[i]._mtwid = _e[i]._mtwid;
    _e[i]._caent = 0;
    if(_e[i]._ca) { delete[] _e[i]._ca; _e[i]._ca = 0; }
    if(_e[i]._ca_mate)
      { delete[] _e[i]._ca_mate; _e[i]._ca_mate = 0; }
  }
  _magic = 0;

#ifdef DEBUG
  _ent_a = 0;
  _ref = 0;
  _hit = 0;
  _col = 0;
  _count = 0;
  _sol = 0;
#endif

  // set cache skip level
  int i = 0;
  while(1)
  {
    i++; if(i >= _m) break;
    _e[i++]._casize = 0; if(i >= _m) break;
    _e[i++]._casize = 0; if(i >= _m) break;
    if(i+1 < _m && _e[i]._mtwid >= _e[i+1]._mtwid)
      { _e[i++]._casize = 0; if(i >= _m) break; }
  }

  // malloc MateCache
  for(int i=0; i<_m; i++)
  {
    if(_e[i]._casize != 0)
    {
      if(!(_e[i]._ca = new GraphSupport::CacheEntry[_e[i]._casize]))
        return 1;
      if(!(_e[i]._ca_mate = new GS_v[_e[i]._mtwid*_e[i]._casize]))
        return 1;
      for(int j=0; j<_e[i]._mtwid*_e[i]._casize; j++)
        _e[i]._ca_mate[j] = 0;
    }
  }

  // malloc and init cfg
  for(int i=0; i<_m; i++)
  {
    if(_e[i]._cfg) { delete[] _e[i]._cfg; _e[i]._cfg = 0; }
    if(!(_e[i]._cfg = new GS_v[_n])) return 1;

    for(int j=0; j<_n; j++) _e[i]._cfg[j] = j+1;
  }

  _e[0]._f = G->_f; // for ZDD-constrained enumeration

/* 
  for(int i=0; i<_n; i++) cout << (int)_v[i]._deg << "\n";
  for(int i=0; i<_m; i++)
  {
    cout << (int)_e[i]._mtwid << " ";
    for(int j=0; j<_e[i]._mtwid; j++) cout << (int)_e[i]._map[j];
    cout << "\n";
  }
  cout << (int)G->_maxwid << "\n\n";
*/

  return 0;
}

static ZBDD EnumCycles(const GS_e);
ZBDD EnumCycles(const GS_e ix)
{
#ifdef DEBUG
  _count++;
#endif

  if(_e[ix]._f == 0) return 0; // ZDD-constrained enumeration

  ZBDD h = 0;
  ZBDD h0, h1;
  if(ix > 0 && (h = CacheCheck(ix-1)) != -1)
    return h; // cache hit

  GS_v ev[2];
  ev[0] = _e[ix]._ev[0];
  ev[1] = _e[ix]._ev[1];

  // _e[ix]._cfg copy 
  if(ix > 0)
  {
    for(int k=0; k<2; k++)
    {
      _e[ix]._cfg[ev[0]-1] = _e[ix-1]._cfg[ev[0]-1];
      _e[ix]._cfg[ev[1]-1] = _e[ix-1]._cfg[ev[1]-1];
    }
    for(int i=0; i<_e[ix-1]._mtwid; i++)
    {
      GS_v v = _e[ix-1]._map[i];
      _e[ix]._cfg[v-1] = _e[ix-1]._cfg[v-1];
    }
  }

  // check for h0
  for(int i=0; i<2; i++)
  {
    if((_e[ix]._io[i] & 2) != 0) // frontier exit 
    {
      GS_v v = ev[i];
      GS_v v_mate = _e[ix]._cfg[v-1];
      if(v_mate != 0 && (G->_hamilton || v_mate != v))
          { h0 = 0; goto skip0; } // invalid terminal 
    }
  }
  if(_e[ix]._preset == GS_fix1) { h0 = 0; goto skip0; } // preset to 1
  if(ix == _m-1 && !G->_hamilton) { h0 = 0; goto skip0; } // empty subgraph

//cout << ix << ": ";
//for(int j=0; j<_n; j++) cout << (int)_e[ix]._cfg[j] << " ";
//cout << "c0\n";
  _e[ix+1]._f = _e[ix]._f.OffSet(BDD_VarOfLev(_m-ix)); // ZDD-constrained enu.
  h0 = EnumCycles(ix+1);

skip0:
  // check for h1 
  GS_v mate_ev[2];
  if((mate_ev[0] = _e[ix]._cfg[ev[0]-1]) == 0 ||
     (mate_ev[1] = _e[ix]._cfg[ev[1]-1]) == 0) 
    { h1 = 0; goto skip1; } // branch
  if(_e[ix]._preset == GS_fix0) { h1 = 0; goto skip1; } // preset to 0
  if(mate_ev[0] == ev[1] && mate_ev[1] == ev[0])
  {
    for(int i=0; i<_e[ix]._mtwid; i++)
    {
      GS_v v = _e[ix]._map[i];
      if(v == ev[0]) continue;
      if(v == ev[1]) continue;
      GS_v v_mate = _e[ix]._cfg[v-1];
      if(v_mate != 0 && (G->_hamilton || v_mate != v))
          { h1 = 0; goto skip1; } // invalid terminal 
    }
    if(!G->_hamilton || ix >= G->_lastin) // a solution found
    {
      if((_e[ix]._f.OnSet0(BDD_VarOfLev(_m-ix)) & 1) != 0) // ZDD-Constrained enum.
      {
#ifdef DEBUG
        _sol++;
#endif
        h1 = 1; goto skip1;
      }
      h1 = 0; goto skip1; 
    }
  }

  // update for h1
  for(int k=0; k<2; k++)
    if(mate_ev[k] == ev[k]) _e[ix]._cfg[ev[k]-1] = mate_ev[1-k];
    else
    {
      _e[ix]._cfg[mate_ev[k]-1] = mate_ev[1-k];
      _e[ix]._cfg[ev[k]-1] = 0;
    }

  // check for h1 
  for(int k=0; k<2; k++)
  {
    if((_e[ix]._io[k] & 2) != 0) // frontier exit
    {
      GS_v v = ev[k];
      GS_v v_mate = _e[ix]._cfg[v-1];
      if(v_mate != 0 && (G->_hamilton || v_mate != v))
        { h1 = 0; goto skip1; } // invalid terminal 
    }
  }
//cout << ix << ": ";
//for(int j=0; j<_n; j++) cout << (int)_e[ix]._cfg[j] << " ";
//cout << "c1\n";
  _e[ix+1]._f = _e[ix]._f.OnSet0(BDD_VarOfLev(_m-ix)); // ZDD-constrained enu.
  h1 = EnumCycles(ix+1);

skip1:
  h = h0 + h1.Change(BDD_VarOfLev(_m - ix));
  if(ix > 0) CacheEnter(ix-1, h);
  return h;
}

ZBDD GraphSupport::SimPaths(const GS_v s, const GS_v t)
{
  if(_n >= 255) return -1; // 8bit-v;
  if(s < 1 || s > _n) return -1;
  if(t < 1 || t > _n) return -1;
  if(s == t) return 1;

  GraphSupport g;
  g.Init(_n, _m+1);
  g._e[0]._ev[0] = s;
  g._e[0]._ev[1] = t;
  g.FixEdge(0, GS_fix1);
  for(int i=0; i<_m; i++)
  {
    g._e[i+1]._ev[0] = _e[i]._ev[0];
    g._e[i+1]._ev[1] = _e[i]._ev[1];
  }
  g.SetHamilton(_hamilton);

  int v = g.BDDvarOfEdge(0);
  g.SetCond(_f.Change(v)); // for ZDD-constrained enum.
  return g.SimCycles().OnSet0(v);
}

ZBDD GraphSupport::SimCycles()
{
  if(_n >= 255) return -1; // 8bit-v;
  if(_n < 3) return 0;

  G = this;
  if(EnumCyclesInit()) return -1;

  ZBDD h = EnumCycles(0);

#ifdef DEBUG
  int a = 0;
  for(int i=0; i<_m; i++)
  {
    int size = _e[i]._casize;
    a += size;
    cout << i << " " << size << "\n";
  }
  cout << "count: " << _count << "\n";
  cout << "mate:" << a << "\n";
  cout << "ref: " << _ref << "\n";
  cout << "hit: " << _hit << "\n";
  cout << "col: " << _col << "\n";
  cout << "ent: " << _ent_a << "\n";
  cout << "sol: " << _sol << "\n";
#endif

  return h;
}

