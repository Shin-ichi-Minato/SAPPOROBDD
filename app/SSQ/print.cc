// SSQ Print part (v0.1)
// Shin-ichi MINATO (May 17, 2015)

#include <cstdio>
#include <iostream>
#include <cstring>
#include "SeqBDD.h"
#include "ssq.h"
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
int Plot(SeqBDD v, int inv)
{
  if(v == SeqBDD(-1)) return 1;
  ZBDD v2 = v.GetZBDD();
  v2.XPrint();
  return 0;
}
#endif

static int* Seq;
static bddword Index;
static int Flag;

static void PrintSeq(SeqBDD);
static void PrintSeq(SeqBDD f)
{
  if((f & 1)== 1)
  {
    if(Flag > 0) bout << " +";
    if(Index == 0) bout << " 1";
    else for(int i=0; i<Index; i++)
    {
      bout.Delimit();
      bout << VTable.GetName(Seq[i]);
    }
    Flag = 1;
    f -= 1;
  }
  if(f == 0) return;
  int top = f.Top();
  SeqBDD f1 = f.OnSet0(top);
//  Seq[Index] = BDD_LevOfVar(top);
  Seq[Index] = top;
  Index++;
  PrintSeq(f1);
  Index--;
  SeqBDD f0 = f.OffSet(top);
  PrintSeq(f0);
}

void ssqprint(SeqBDD p)
{
  if(p == -1)
  {
    bout << "(undefined)";
    bout.Return();
    return;
  }
  if(p == 0)
  {
    bout << " 0";
    bout.Return();
    return;
  }
  bddword len = p.Len();
  Seq = new int[len];
  Index = 0;
  Flag = 0;
  PrintSeq(p);
  delete[] Seq;
  bout.Return();
}

