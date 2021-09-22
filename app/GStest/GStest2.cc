#include <iostream>
#include <cstring>
#include "GraphSupport.h"
using namespace std;

int main(int argc, char *argv[])
{
  BDDV_Init(256, 50000000);

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
//g.Print();
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
  h = g.SimPaths(1, g._n);
  h.Print();
  //h.PrintPla();

/*
  CtoI hv = h;
  char s[256];
  hv.CountTerms().StrNum10(s);
  cout << "terms: " << s << "\n";
*/
  return 0;
}

