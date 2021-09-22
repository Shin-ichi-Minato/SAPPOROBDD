// Perm Print part (v0.1)
// Shin-ichi MINATO (Feb. 26, 2011)

#include <cstdio>
#include <iostream>
#include <cstring>
#include "PiDD.h"
#include "perm.h"
using namespace std;

#define LINE 70

static int P_OR;

static int LitIP;
static int* IPlist;

BOut::BOut() { _column = 0; }

BOut& BOut::operator << (const char* str)
{
  _column += strlen(str);
  cout << str;
  return *this;
}

void BOut::Delimit()
{
  if(_column >= LINE)
  {
    _column = 2;
    cout << "\n  ";
    cout.flush();
  }
  else
  {
    _column++;
    cout << " ";
  }
}

void BOut::Delimit0()
{
  if(_column >= LINE)
  {
    _column = 2;
    cout << "\n  ";
    cout.flush();
  }
}

void BOut::Return()
{
  _column = 0;
  cout << "\n";
  cout.flush();
}

BOut bout;

#ifndef B_STATIC
int Plot(PiDD v, int inv)
{
  if(v == PiDD(-1)) return 1;
  ZBDD v2 = v.GetZBDD();
  v2.XPrint();
  return 0;
}
#endif

static int* VarMap;
static int Flag;
static int Depth;

static void Enum(PiDD, int);
static void Enum(PiDD p, int dim)
{
  if(p == -1) return;
  if(p == 0) return;
  if(p == 1)
  {
    if(Flag) { bout.Delimit(); bout << "+ "; }
    else Flag = 1;
    int d0 = 0;
    for(int i=0; i<dim; i++) if(VarMap[i] != i + 1) d0 = i + 1;
    if(d0 == 0) bout << "1";
    else
    {
      bout << "[";
      for(int i=0; i<d0; i++)
      {
        if(i > 0) bout.Delimit();
        int a = VarMap[i];
        if(a == i + 1) bout << "-";
        else bout << VTable.GetName(a);
      }
      bout << "]";
    }
    return;
  }
  int x = p.TopX();
  int y = p.TopY();
  PiDD p1 = p.Cofact(x, y);
  PiDD p0 = p - p1.Swap(x, y);
  Enum(p0, dim);
  int t;
  t = VarMap[x-1]; VarMap[x-1] = VarMap[y-1]; VarMap[y-1] = t;
  Enum(p1, dim);
  t = VarMap[x-1]; VarMap[x-1] = VarMap[y-1]; VarMap[y-1] = t;
}

void PermEnum(PiDD p)
{
  bout << " ";
  if(p == -1)
  {
    bout << "(undefined)";
    bout.Return();
    return;
  }
  if(p == 0)
  {
    bout << "0";
    bout.Return();
    return;
  }
  if(p == 1)
  {
    bout << "1";
    bout.Return();
    return;
  }

  Flag = 0;
  int dim = p.TopX();
  VarMap = new int[dim];
  for(int i=0; i<dim; i++) VarMap[i] = i+1;
  Enum(p, dim);
  delete[] VarMap;
  bout.Return();
}

static int* MapX;
static int* MapY;
static void Enum2(PiDD);
static void Enum2(PiDD p)
{
  if(p == -1) return;
  if(p == 0) return;
  if(p == 1)
  {
    if(Flag) { bout.Delimit(); bout << "+ "; }
    else Flag = 1;
    if(Depth == 0) bout << "1";
    else
    {
      for(int i=0; i<Depth; i++)
      {
        int x = MapX[Depth - i - 1];
        int y = MapY[Depth - i - 1];
	bout << "(" << VTable.GetName(x) << ":"
	     << VTable.GetName(y) << ")";
	bout.Delimit0();
      }
    }
    return;
  }
  int x = p.TopX();
  int y = p.TopY();
  PiDD p1 = p.Cofact(x, y);
  PiDD p0 = p - p1.Swap(x, y);
  Enum2(p0);
  MapX[Depth] = p.TopX();
  MapY[Depth] = p.TopY();
  Depth++;
  Enum2(p1);
  Depth--;
}

void PermEnum2(PiDD p)
{
  bout << " ";
  if(p == -1)
  {
    bout << "(undefined)";
    bout.Return();
    return;
  }
  if(p == 0)
  {
    bout << "0";
    bout.Return();
    return;
  }
  if(p == 1)
  {
    bout << "1";
    bout.Return();
    return;
  }

  Flag = 0;
  int dim = p.TopX();
  MapX = new int[dim];
  MapY = new int[dim];
  Depth = 0;
  Enum2(p);
  bout.Return();
  delete[] MapY;
  delete[] MapX;
}
