#include <iostream>
#include <cstring>
#include "GraphSupport.h"
using namespace std;

int main(int argc, char *argv[])
{
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
  g.Pack();
  g.Print();
  return 0;
}

