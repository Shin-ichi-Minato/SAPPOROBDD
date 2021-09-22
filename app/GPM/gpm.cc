/****************************************
 * Graph Path Miner ver 1.00 - Main     *
 * (C) Shin-ichi MINATO (Sep. 13, 2020) *
 ****************************************/

#include <cstdio>
#include <iostream>
#include <cstring>
#include "ZBDD.h"
#include "BDDCT.h"
#include "GBase.h"
#include "gpm.h"
using namespace std;

static GBase* G = 0;
static BDDCT* CT = 0;
static int* L = 0;
static int K = 0;
static int SV = 0;
static int TV = 0;
static bddcost Cost = 0;

static int NextV(const int, const int);
int NextV(const int v, const int lastv)
{
  int v2 = 0;
  for(int i=0; i<K; i++)
  {
    int ix = G->_m - L[i];
    if(G->_e[ix]._ev[0] == v && (v2=G->_e[ix]._ev[1]) != lastv)
      break;
    if(G->_e[ix]._ev[1] == v && (v2=G->_e[ix]._ev[0]) != lastv)
      break;
  }
  return v2;
}

static void PrintPaths(const ZBDD &);
void PrintPaths(const ZBDD& f)
{
  if(f == 0) return;
  if(f == 1)
  {
    cout << Cost << ": ";
    int nv, v, lastv;
    if(SV == 0)
    {
      TV = G->_e[G->_m - L[0]]._ev[0];
      cout << TV << "-";
      nv = NextV(TV, 0);
      if(nv == 0) return;
      lastv = TV;
    }
    else
    {
      cout << SV << "-";
      nv = NextV(SV, 0);
      if(nv == 0) return;
      lastv = SV;
    }
    v = nv;
    while(v != TV)
    {
      cout << v << "-";
      nv = NextV(v, lastv);
      if(nv == 0) return;
      lastv = v;
      v = nv;
    }
    cout << TV << "\n";
    return;
  }
  int top = f.Top();
  int tlev = BDD_LevOfVar(top);
  PrintPaths(f.OffSet(top));
  L[K++] = tlev;
  Cost += CT->CostOfLev(tlev);
  PrintPaths(f.OnSet0(top));
  K--;
  Cost -= CT->CostOfLev(tlev);
  return;
}

int main(int argc, char *argv[])
{
  FILE *fp = 0;
  GB_v sv = 0;
  GB_v tv = 0;
  bddcost bound = 0;
  int prn = 0;
  int ham = 0;
  int cycle = 0;
  int e = 0;
  if(argc < 3) e = 1;
  else
  {
    if((fp = fopen(argv[1], "r")) == 0)
    {
      cout << "filename??\n";
      return 1;
    }
    bound = strtol(argv[2], NULL, 10);
    int px = 2;
    while(++px < argc)
    {
      if(!strcmp(argv[px], "-p")) prn = 1;
      else if(!strcmp(argv[px], "-h")) ham = 1;
      else if(!strcmp(argv[px], "-c")) cycle = 1;
      else if(!strcmp(argv[px], "-t"))
      {
        if(++px >= argc) e = 1;
        else
	{
	  sv = strtol(argv[px], NULL, 10);
          if(++px >= argc) e = 1;
          else tv = strtol(argv[px], NULL, 10);
	}
      }
      else e = 1;
    }
  }
  if(e)
  {
    cerr << "gpm <filename> <cost_bound> [<flag> [<number>]* ]*\n";
    cerr << "<flag>\n";
    cerr << "-p             print solutions\n";
    cerr << "-h             set hamilton\n";
    cerr << "-c             set cycle\n";
    cerr << "-t <s> <t>     set termninals <s> and <t>\n";
    return 1;
  }

  cerr << "**** Graph Path Miner (v1.0) ****\n";
  cerr.flush();

  if(BDDV_Init(256, 40000000))
  {
    cerr << "malloc failed.\n";
    return 1;
  }

  GBase g;
  g.Import(fp);
  if(ham) g.SetHamilton(1);
  if(sv < 1 || sv > g._n) sv = 1;
  if(tv < 1 || tv > g._n) tv = g._n;

  cerr << "V: " << g._n << "    E: " << g._m << "\n";

  ZBDD f;
  if(cycle) f = g.SimCycles();
  else f = g.SimPaths(sv, tv);

  if(f == 0)
  {
    cerr << "no solutions.\n";
    return 0;
  }

  BDDCT ct;
  ct.Alloc(g._m);
  for(int i=0; i<g._m; i++)
    ct.SetCost(i, g._e[i]._cost);

  char s[256];
  bddword card = f.Card();
  if(cycle) cerr << "all cycles:     ";
  else cerr << "all paths:      ";
  if(card < ZBDD(-1).GetID())
    cerr << card << "\n";
  else 
  {
    f.CardMP16(s);
    cerr << "0x" << s << "\n";
  }
  bddword size = f.Size();
  cerr << "(ZDD size)      " << size << "\n";

  bddcost min = ct.MinCost(f);
  cerr << "cost min:       " << min << "\n";

  bddcost max = ct.MaxCost(f);
  cerr << "cost max:       " << max << "\n";

  cerr << "cost bound:     " << bound << "\n";

  ZBDD h = ct.ZBDD_CostLE(f, bound);

  card = h.Card();
  if(cycle) cerr << "bounded cycles: ";
  else cerr << "bounded paths:  ";
  if(card < ZBDD(-1).GetID())
    cerr << card << "\n";
  else 
  {
    h.CardMP16(s);
    cerr << "0x" << s << "\n";
  }
  size = h.Size();
  cerr << "(ZDD size)      " << size << "\n";

  if(prn)
  {
    G = &g;
    CT = &ct;
    L = new int[g._m];
    if(cycle){ SV = 0; TV = 0; }
    else { SV = sv; TV = tv; }
    Cost = 0;
    K = 0;
    PrintPaths(h);
    delete[] L;
  }

  return 0;
}

