/****************************************
 * Graph Path Miner ver 2.0 - Main      *
 * (C) Shin-ichi MINATO (Oct. 16, 2021) *
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
static GB_e* L = 0;
static GB_e K = 0;
static GB_v SV = 0;
static GB_v TV = 0;
static bddcost Cost = 0;

static int NextV(const GB_v, const GB_v);
int NextV(const GB_v v, const GB_v lastv)
{
  int v2 = 0;
  for(GB_e i=0; i<K; i++)
  {
    GB_e ix = G->_m - L[i];
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
    GB_v nv, v, lastv;
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

static void PrintEdges(const ZBDD &);
void PrintEdges(const ZBDD& f)
{
  if(f == 0) return;
  if(f == 1)
  {
    cout << Cost << ":";
    for(GB_e i=0; i<K; i++) cout << " e" << (G->_m - L[i]);
    cout << "\n";
    return;
  }
  int top = f.Top();
  int tlev = BDD_LevOfVar(top);
  PrintEdges(f.OffSet(top));
  L[K++] = tlev;
  Cost += CT->CostOfLev(tlev);
  PrintEdges(f.OnSet0(top));
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
  int all = 0;
  int repeat = 1;
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
      else if(!strcmp(argv[px], "-a")) all = 1;
      else if(!strcmp(argv[px], "-r"))
      {
        if(++px >= argc) e = 1;
        else repeat = strtol(argv[px], NULL, 10);
      }
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
    cerr << "-a             set all combinations\n";
    cerr << "-t <s> <t>     set termninals <s> and <t>\n";
    cerr << "-r <n>         set repeat <n> times\n";
    return 1;
  }

  cerr << "**** Graph Path Miner (v2.0) ****\n";
  cerr.flush();

  if(BDDV_Init(256, 60000000))
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
  if(all)
  {
    f = 1;
    for(int i=0; i<g._m; i++)
    {
      BDD_NewVar();
      f += f.Change(BDD_VarOfLev(i+1));
    }
  }
  else if(cycle) f = g.SimCycles();
  else f = g.SimPaths(sv, tv);

  if(f == 0)
  {
    cerr << "no solutions.\n";
    return 0;
  }

  BDDCT ct;
  ct.Alloc(g._m);
  for(GB_e i=0; i<g._m; i++)
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

  bddcost acc_worst, rej_best;
  ZBDD h;
  if(prn)
  {
    G = &g;
    CT = &ct;
    L = new GB_e[g._m];
    if(cycle) SV = 0; 
    else { SV = sv; TV = tv; }
  }
  for(int i=0; i<repeat; i++)
  {
    cerr << "----\ncost bound:     " << bound << "\n";

    h = ct.ZBDD_CostLE(f, bound, acc_worst, rej_best);
    cerr << "accept_worst:   ";
    if(acc_worst != bddcost_null) cerr << acc_worst << "\n";
    else cerr << "-\n";
    cerr << "reject_best:    ";
    if(rej_best != bddcost_null) cerr << rej_best << "\n";
    else cerr << "-\n";
  
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
      Cost = 0;
      K = 0;
      if(all) PrintEdges(h);
      else PrintPaths(h);
    }
    if(rej_best == bddcost_null) break;
    bound = rej_best;
  }

  if(prn) delete[] L;
  return 0;
}

