#include <iostream>
#include <cstring>
#include "GraphSupport.h"
using namespace std;

int main(int argc, char *argv[])
{
  if(BDDV_Init(256, 60000000))
  {
    cerr << "malloc failed.\n";
    return 1;
  }

  FILE *fp = 0;
  if(argc == 1) fp = stdin;
  else
  {
    if((fp = fopen(argv[1], "r")) == 0)
    {
      cout << "filename??\n";
      return 1;
    }
  }

  GraphSupport g;
  g.Import(fp);

  /*
  ZBDD f = 1;
  for(int i=0; i<g._m; i++)
  {
    f += f.Change(g.BDDvarOfEdge(g._m-1-i));
  }
  f.Print();
  f = f.PermitSym(g._n-1);
  f.Print();
  f -= f.PermitSym(g._n-2);
  f.Print();
  g.SetCond(f);
  */

  g.SetHamilton(0);
  ZBDD h;
  //h = g.SimCycles();
  h = g.SimPaths(1, g._n);
  h.Print();
  //h.PrintPla();

  char s[129];
  h.CardMP16(s);
  cout << "terms: 0x" << s << "\n";
  return 0;
}

