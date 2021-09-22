#include <iostream>
#include <cstring>
#include "GraphSupport.h"
using namespace std;

int main(int argc, char *argv[])
{
  int n = 0;
  if(argc > 1) n = strtol(argv[1], NULL, 10);
  GraphSupport g;
  g.SetGrid(n, n);
  g.Print();
  return 0;
}

