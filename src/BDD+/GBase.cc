/****************************************
 * Graph Base class (SAPPORO-1.92)      *
 * (Main part)                          *
 * (C) Shin-ichi MINATO (Oct. 16, 2021) *
 ****************************************/

#include "GBase.h"

using std::cout;
using std::cerr;

//-------------- Class methods of GBase -----------------

GBase::GBase()
{
  _n = 0;
  _m = 0;
  _v = 0;
  _e = 0;
  _hamilton = 0;
  _f = -1; // for ZDD-constrained enumeration
}

GBase::~GBase()
{
  if(_v) delete[] _v;
  if(_e) delete[] _e;
}

void GBase::FixEdge(const GB_e ix, const char fix)
{ _e[ix]._preset = fix; }

void GBase::SetHamilton(const int hm) { _hamilton = hm; }

void GBase::SetCond(ZBDD f) { _f = f; }
  // for ZDD-constrained enumeration

GBase::Edge::Edge()
{
  _ev[0] = 0;
  _ev[1] = 0;
  _tmp = 0;
  _cost = 1;

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

GBase::Edge::~Edge()
{
  if(_map) delete[] _map;
  if(_cfg) delete[] _cfg;
  if(_ca) delete[] _ca;
  if(_ca_mate) delete[] _ca_mate;
}

int GBase::Init(const int n, const int m)
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

int GBase::Pack()
{
  int i;
  for(i=0; i<_n; i++) _v[i]._tmp = 0;
  GB_v x;
  GB_v nx = 0;
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

int GBase::Import(FILE *strm)
{
  char s[256];

  do if(fscanf(strm, "%s", s) == EOF) return 1;
  while(s[0] == '#');
  int n = strtol(s, NULL, 10);

  do if(fscanf(strm, "%s", s) == EOF) return 1;
  while(s[0] == '#');
  int m = strtol(s, NULL, 10);

  if(Init(n, m)) return 1;

  GB_v v;
  GB_e e = 0;
  do if(fscanf(strm, "%s", s) == EOF) { if(m > 0) e = 1; break; }
  while(s[0] == '#');
  for(int i=0; i<m; i++)
  {
    v = strtol(s, NULL, 10);
    if(v <= 0 || v > n) { e = 1; break; }
    _e[i]._ev[0] = v;
    do if(fscanf(strm, "%s", s) == EOF) { e = 1; break; }
    while(s[0] == '#');
    v = strtol(s, NULL, 10);
    if(v <= 0 || v > n) { e = 1; break; }
    _e[i]._ev[1] = v;
    do
    {
      if(fscanf(strm, "%s", s) == EOF)
      {
        if(i < m - 1) e = 1;
        break;
      }
      if(s[0] == '#' && s[1] == 'c' && s[2] == ':')
        _e[i]._cost = strtol(s+3, NULL, 10);
    } while(s[0] == '#');
  }

  if(e) { Init(0, 0); return 1; }
  return 0;
}

int GBase::SetGrid(const int x, const int y)
{
  if(x < 0||y < 0) return 1;
  if(Init((x+1)*(y+1), 2*x*y+x+y)) return 1;
  GB_e p = 0;
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

void GBase::Print() const
{
  cout << "#n " << (int) _n << "\n";
  cout << "#m " << (int) _m << "\n";
  for(int i=0; i<_m; i++)
    cout << (int) _e[i]._ev[0] << " " << (int) _e[i]._ev[1]
         << " #c:" << _e[i]._cost << "\n";
  cout.flush();
}

inline int GBase::BDDvarOfEdge(const GB_e ix) const
{ 
  while(BDD_TopLev() < _m) BDD_NewVar();
  return BDD_VarOfLev(_m - ix);
}

inline GB_e GBase::EdgeOfBDDvar(const int var) const
{
  while(BDD_TopLev() < _m) BDD_NewVar();
  return _m - BDD_LevOfVar(var);
}

static GBase *G;
static GBase::Vertex *_v;
static GBase::Edge *_e;
static GB_v _n;
static GB_e _m;

static const int HashStart = 1<<2;
static const int HashTry = 1<<4;
static int _magic;

const char GB_fix0 = 1;
const char GB_fix1 = 2;

#ifdef DEBUG
static GB_v _maxwid;
static bddword _ent_a;
static bddword _ref;
static bddword _col;
static bddword _hit;
static bddword _count;
static bddword _sol;
#endif

static int CacheEnlarge(GB_e);
int CacheEnlarge(GB_e ix)
{
  bddword newsize = _e[ix]._casize << 1;

  GBase::CacheEntry* newca = 0;
  if(!(newca = new GBase::CacheEntry[newsize])) return 1;

  GB_v* newca_mate = 0;
  bddword newsize_mate = newsize * _e[ix]._mtwid;
  if(!(newca_mate =  new GB_v[newsize_mate]))
    { delete[] newca; return 1; }

  for(bddword i=0; i<newsize_mate; i++) newca_mate[i] = 0;
  for(bddword j=0; j<_e[ix]._casize; j++) 
  {
    if(_e[ix]._ca[j]._h != -1)
    {
      bddword k = 0;
      bddword id = _e[ix]._ca[j]._f.GetID(); // ZDD-constrained enum.
      k = (id+(id>>10)+(id>>20)); // ZDD-constrained enum.
      for(int i=0; i<_e[ix]._mtwid; i++)
      {
        bddword x = _e[ix]._ca_mate[j * _e[ix]._mtwid + i];
        k ^= (i<<2)^(k<<12)^x^(x<<(2*i+3))^(x<<((11*i+7)&15)); // 8bit-v;
      }
      k &= newsize - 1;
      bddword k0 = k;
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
        newca_mate[k * _e[ix]._mtwid + i] = _e[ix]._ca_mate[j * _e[ix]._mtwid + i];
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

static bddword Hash(const GB_e);
bddword Hash(const GB_e ix)
{
  bddword k = 0;
  bddword id = _e[ix]._f.GetID(); // ZDD-constrained enum.
  k = (id+(id>>10)+(id>>20)); // ZDD-constrained enum.
  for(int i=0; i<_e[ix]._mtwid; i++)
  {
    bddword x = _e[ix]._cfg[_e[ix]._map[i]-1];
    k ^= (i<<2)^(k<<12)^x^(x<<(2*i+3))^(x<<((11*i+7)&15)); // 8bit-v;
  }
  k &= _e[ix]._casize - 1;
  return k;
}

static ZBDD CacheCheck(const GB_e);
ZBDD CacheCheck(const GB_e ix)
{
  if(_e[ix]._casize == 0) return -1;

#ifdef DEBUG
  _ref++;
#endif
  bddword k = Hash(ix);
  for(int t=0; t<HashTry; t++)
  {
    if(_e[ix]._ca[k]._h == -1) return -1;
    int i;
    for(i=0; i<_e[ix]._mtwid; i++)
    {
      if(_e[ix]._ca_mate[k * _e[ix]._mtwid + i]
         != _e[ix]._cfg[_e[ix]._map[i]-1]) break;
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

static void CacheEnter(const GB_e, ZBDD);
void CacheEnter(const GB_e ix, ZBDD h)
{
  if(_e[ix]._casize == 0) return;

  if(++_e[ix]._caent >= _e[ix]._casize &&
     (_e[ix]._caent & 255) == 0 )
  {
    if(CacheEnlarge(ix)) cerr << "enlarge failed.\n";
  }

#ifdef DEBUG
  _ent_a++;
#endif
  bddword k = Hash(ix);
  bddword k0 = k;
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
    _e[ix]._ca_mate[k * _e[ix]._mtwid + i] = _e[ix]._cfg[_e[ix]._map[i]-1];
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
#ifdef DEBUG
  _maxwid = 0;
#endif // DEBUG
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
#ifdef DEBUG
    _maxwid = (_maxwid < _e[i]._mtwid)? _e[i]._mtwid: _maxwid;
#endif // DEBUG
  }

  for(int i=0; i<_m; i++)
  {
    // malloc and init _map

    if(_e[i]._map) { delete[] _e[i]._map; _e[i]._map = 0; }
    if(!(_e[i]._map = new GB_v[_e[i]._mtwid])) return 1;

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
    for(int p=0; p<2; p++)
      if((_e[i]._io[p] & 1) != 0) _e[i]._map[k++] = _e[i]._ev[p];
  }

  // init MateCache
  for(int i=0; i<_m; i++)
  {
    _e[i]._casize = HashStart;
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
  for(i=0; i<_m; i++)
  {
    if(_e[i]._casize != 0)
    {
      if(!(_e[i]._ca = new GBase::CacheEntry[_e[i]._casize]))
        return 1;
      bddword casize_mate = _e[i]._casize * _e[i]._mtwid;
      if(!(_e[i]._ca_mate = new GB_v[casize_mate]))
        return 1;
      for(bddword j=0; j<casize_mate; j++)
        _e[i]._ca_mate[j] = 0;
    }
  }

  // malloc and init cfg
  for(i=0; i<_m; i++)
  {
    if(_e[i]._cfg) { delete[] _e[i]._cfg; _e[i]._cfg = 0; }
    if(!(_e[i]._cfg = new GB_v[_n])) return 1;

    for(int j=0; j<_n; j++) _e[i]._cfg[j] = j+1;
  }

  _e[0]._f = G->_f; // for ZDD-constrained enumeration
  return 0;
}

static ZBDD EnumCycles(const GB_e);
ZBDD EnumCycles(const GB_e ix)
{
#ifdef DEBUG
  _count++;
#endif

  if(_e[ix]._f == 0) return 0; // ZDD-constrained enumeration

  ZBDD h = 0;
  ZBDD h0, h1;
  if(ix > 0 && (h = CacheCheck(ix-1)) != -1)
    return h; // cache hit

  GB_v ev[2];
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
      GB_v v = _e[ix-1]._map[i];
      _e[ix]._cfg[v-1] = _e[ix-1]._cfg[v-1];
    }
  }

  // check for h0
  for(int i=0; i<2; i++)
  {
    if((_e[ix]._io[i] & 2) != 0) // frontier exit 
    {
      GB_v v = ev[i];
      GB_v v_mate = _e[ix]._cfg[v-1];
      if(v_mate != 0 && (G->_hamilton || v_mate != v))
          { h0 = 0; goto skip0; } // invalid terminal 
    }
  }
  if(_e[ix]._preset == GB_fix1) { h0 = 0; goto skip0; } // preset to 1
  if(ix == _m-1 && !G->_hamilton) { h0 = 0; goto skip0; } // empty subgraph

//cout << ix << ": ";
//for(int j=0; j<_n; j++) cout << (int)_e[ix]._cfg[j] << " ";
//cout << "c0\n";
  _e[ix+1]._f = _e[ix]._f.OffSet(BDD_VarOfLev(_m-ix)); // ZDD-constrained enu.
  h0 = EnumCycles(ix+1);

skip0:
  // check for h1 
  GB_v mate_ev[2];
  if((mate_ev[0] = _e[ix]._cfg[ev[0]-1]) == 0 ||
     (mate_ev[1] = _e[ix]._cfg[ev[1]-1]) == 0) 
    { h1 = 0; goto skip1; } // branch
  if(_e[ix]._preset == GB_fix0) { h1 = 0; goto skip1; } // preset to 0
  if(mate_ev[0] == ev[1] && mate_ev[1] == ev[0])
  {
    for(int i=0; i<_e[ix]._mtwid; i++)
    {
      GB_v v = _e[ix]._map[i];
      if(v == ev[0]) continue;
      if(v == ev[1]) continue;
      GB_v v_mate = _e[ix]._cfg[v-1];
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
      GB_v v = ev[k];
      GB_v v_mate = _e[ix]._cfg[v-1];
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

ZBDD GBase::SimPaths(const GB_v s, const GB_v t)
{
  //if(_n >= 255) return -1; // 8bit-v;
  if(_n >= 65535) return -1; // 16bit-v;
  if(_n < 2 || _m < 1) return 0;
  if(s < 1 || s > _n) return -1;
  if(t < 1 || t > _n) return -1;
  if(s == t) return 0;

  GBase g;
  g.Init(_n, _m+1);
  g._e[0]._ev[0] = s;
  g._e[0]._ev[1] = t;
  g.FixEdge(0, GB_fix1);
  for(int i=0; i<_m; i++)
  {
    g._e[i+1]._ev[0] = _e[i]._ev[0];
    g._e[i+1]._ev[1] = _e[i]._ev[1];
  }
  g.SetHamilton(_hamilton);

  int v = g.BDDvarOfEdge(0);
  return g.SimCycles().OnSet0(v);
}

ZBDD GBase::SimCycles()
{
  //if(_n >= 255) return -1; // 8bit-v;
  if(_n >= 65535) return -1; // 16bit-v;
  if(_n < 3 || _m < 3) return 0;

  G = this;
  if(EnumCyclesInit()) return -1;

  ZBDD h = EnumCycles(0);

#ifdef DEBUG
  bddword a = 0;
  for(int i=0; i<_m; i++)
  {
    bddword size = _e[i]._casize;
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

