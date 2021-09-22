#include <cstdio>
#include <iostream>
#include <cstring>
#include "SOP.h"
using namespace std;

int main(int argc, char *argv[])
{
  cerr << "*** ISOP: Irredundant Sum-Of-Products form generator ***\n";
  cerr.flush();
  if(BDDV_Init(256, 40000000))
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
      cerr << "filename??\n";
      return 1;
    }
  }

  BDDV v = BDDV_ImportPla(fp, 1);
  if(v == BDDV(-1))
  {
    cerr << "file read error.\n";
    return 1;
  }
  int top = BDD_LevOfVar(v.Top());
  int len = v.Len();
  cerr << "source file: i:" << top/2 << " o:" << len/2;
  cerr << " BDDsize:" << v.Size() << "\n";
  cerr.flush();
  BDDV v1 = v.Part(0, len/2);
  BDDV v2 = v.Part(len/2, len/2);
  SOPV sv = SOPV_ISOP(v1, v2);
  cerr << "ISOP form: product:" << sv.Cube()
       << " literal:" << sv.Lit()
       << " ZDDsize:" << sv.GetZBDDV().Size() << "\n";
  sv.PrintPla();

  return 0;
}



