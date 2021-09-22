// VSOP Print (v1.36)
// Shin-ichi MINATO (Dec. 18, 2010)

#include <cstdio>
#include <iostream>
#include <cstring>
#include "CtoI.h"
#include "vsop.h"
#include "ZBDDDG.h"
using namespace std;

#define LINE 70

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

void BOut::Return()
{
  _column = 0;
  cout << "\n";
  cout.flush();
}

BOut bout;

static int PutNum(CtoI a, int base);
int PutNum(CtoI a, int base)
{
  if(a.TopItem() > 0) a = a.MaxVal();
  int d = a.TopDigit() / 3 + 14;
  char* s = new char[d];
  if(s == 0) return 1;

  int err;
  if(base == 16) err = a.StrNum16(s);
  else err = a.StrNum10(s);
  if(err == 1)
  {
    delete[] s;
    return 1;
  }
  int len = strlen(s);
  bout << s;
  delete[] s;
  return 0;
}
	
static int Depth;
static int* S_Var;
static int PFflag;
static int PF(CtoI, int);
static int PF(CtoI a, int base)
{
  if(a.IsConst())
  {
    if(a.TopDigit() & 1) { bout.Delimit(); bout << "-"; a = -a; }
    else if(PFflag == 1) { bout.Delimit(); bout << "+"; }

    PFflag = 1;
    int c1 = (a != 1);
    if(c1 || Depth == 0)
    {
      bout.Delimit();
      if(PutNum(a, base) == 1) return 1;
    }
    for(int i=0; i<Depth; i++)
    {
      bout.Delimit();
      bout << VTable.GetName(S_Var[i]);
    }
    return 0;
  }

  int v = a.TopItem();
  CtoI b = a.Factor1(v);
  if(b == CtoI_Null()) return 1;
  S_Var[Depth] = v;
  Depth++;
  if(PF(b, base) == 1) return 1;
  Depth--;
  b = a.Factor0(v);
  if(b == 0) return 0;
  if(b == CtoI_Null()) return 1;
  return PF(b, base);
}

int PrintCtoI(CtoI a)
{
  if(a == CtoI_Null()) return 1;

  if(a == 0) bout << " 0";
  else
  {
    int lev = BDD_LevOfVar(a.TopItem());
    Depth = 0;
    S_Var = new int[lev];
    PFflag = 0;
    int err = PF(a, 10);
    delete[] S_Var;
    if(err == 1)
    {
      bout << "...";
      bout.Return();
      return 1;
    }
  }
  bout.Return();
  return 0;
}

int PrintCtoI_16(CtoI a)
{
  if(a == CtoI_Null()) return 1;

  if(a == 0) bout << " 0";
  else
  {
    int lev = BDD_LevOfVar(a.TopItem());
    Depth = 0;
    S_Var = new int[lev];
    PFflag = 0;
    int err = PF(a, 16);
    delete[] S_Var;
    if(err == 1)
    {
      bout << "...";
      bout.Return();
      return 1;
    }
  }
  bout.Return();
  return 0;
}

int PrintDigital(CtoI a)
{
  int d = a.TopDigit();
  for(int i=d; i>=0; i--)
  {
    if(d > 1)
    {
      char s[10];
      sprintf(s, "%3d:", i);
      bout << s;
    }
    if(PrintCtoI(a.Digit(i)) == 1) return 1;
  }
  return 0;
}

int PrintCase(CtoI a)
{
  char s[80];
  while(CtoI_GT(a, 0) != 0)
  {
    if(a == CtoI_Null())
    {
      bout << "...";
      bout.Return();
      return 1;
    }
    CtoI b = a.MaxVal();
    b.StrNum10(s);
    bout << s << ": ";
    CtoI c = a.EQ_Const(b);
    PrintCtoI(c);
    a = a.FilterElse(c);
  }
  while(CtoI_LT(a, 0) != 0)
  {
    if(a == CtoI_Null())
    {
      bout << "...";
      bout.Return();
      return 1;
    }
    CtoI b = a.MinVal();
    b.StrNum10(s);
    bout << s << ": ";
    CtoI c = a.EQ_Const(b);
    PrintCtoI(c);
    a = a.FilterElse(c);
  }
  return 0;
}

static void PutCode(int, int);
void PutCode(int num, int digit) //  num < 8, digit <=3
{
  for(int i=3; i>=0; i--)
    if(i >= digit) bout << " ";
    else if((num & (1 << i)) == 0) bout << "0";
    else bout << "1";
}

static int MapVar[6];

static int MapNum(CtoI a);
int MapNum(CtoI a)
{
  int ovf = 0;
  if(a.TopItem() > 0)
  {
    a = a.MaxVal();
    ovf = 1;
  }
  int d = a.TopDigit() / 3 + 14;
  char* s = new char[d];
  if(s == 0) return 1;

  int err;
  err = a.StrNum10(s);
  if(err == 1)
  {
    delete[] s;
    return 1;
  }
  int len = strlen(s);
  if(ovf == 0)
  {
    for(int i=0; i<5-len; i++) bout << " ";
    bout << " " << s;
  }
  else
  {
    for(int i=0; i<4-len; i++) bout << " ";
    bout << "(" << s << ")";
  }
  delete[] s;
  return 0;
}
	
static int Map(CtoI, int);
int Map(CtoI a, int dim)
{
  if(a == CtoI_Null()) return 1;
  int x, y;
  switch(dim)
  {
  case 0:
    if(MapNum(a) == 1) return 1;
    bout.Return();
    return 0;
  case 1:
    bout << " " << VTable.GetName(MapVar[0]);
    bout.Return();
    PutCode(0, 1);
    bout << " |";
    if(MapNum(a.Factor0(MapVar[0])) == 1) return 1;
    bout.Return();
    PutCode(1, 1);
    bout << " |";
    if(MapNum(a.Factor1(MapVar[0])) == 1) return 1;
    bout.Return();
    return 0;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    y = dim / 2;
    x = dim - y;
    break;
  default: ;
  }
  int mx = 1 << x;
  int my = 1 << y;
  for(int i=0; i<y; i++)
    bout << " " << VTable.GetName(MapVar[i]);
  bout << " :";
  for(int i=y; i<dim; i++)
  	bout << " " << VTable.GetName(MapVar[i]);
  bout.Return();
  bout << "    ";
  for(int i=0; i<mx; i++)
  {
    if(i == 0 || i == 4) bout << " |";
    int m = i ^ (i >> 1);
    bout << "  ";
    PutCode(m, x);
  }
  bout.Return();
  for(int j=0; j<my; j++)
  {
    if(j == 4)
    {
      bout << "     |";
      if(x == 3) bout << "                         |";
      bout.Return();
    }
    int n = j ^ (j >> 1);
    PutCode(n, y);
    n <<= x;
    for(int i=0; i<mx; i++)
    {
      if(i == 0 || i == 4) bout << " |";
      int m = n | (i ^ (i >> 1));
      CtoI ax = a;
      for(int k=0; k<dim; k++)
        if((m & (1 << (dim-k-1))) == 0)
          ax = ax.Factor0(MapVar[k]);
      	else	ax = ax.Factor1(MapVar[k]);
      if(MapNum(ax) == 1) return 1;
    }
    bout.Return();
  }
  return 0;
}
			
int MapAll(CtoI a)
{
  int i=0;
  int n = VTable.Used();
  for(int j=0; j<n; j++)
  {
    MapVar[i++] = BDD_VarOfLev(n-j);
    if(i == 6) break;
  }
  return Map(a, i);
}

int MapSel(CtoI a)
{
  int i=0;
  int n = VTable.Used();
  for(int j=0; j<n; j++)
  {
    int var = BDD_VarOfLev(n-j);
    if(a == CtoI_Null()) return 1;
    if(a == a.Factor0(var)) continue;
    MapVar[i++] = var;
    if(i == 6) break;
  }
  return Map(a, i);
}

static void PrintD(ZBDDDG *, bddword);
void PrintD(ZBDDDG* dg, bddword ndx)
{
  ZBDDDG_Tag tag, tag2;
  tag.Set(dg, ndx);
  bddword ndx0, ndx2;
  int top;
  switch(tag.Type())
  {
  case ZBDDDG_C0:
    bout << "0";
    break;
  case ZBDDDG_P1:
    bout << "OR(";
    bout.Delimit();
    ndx0 = tag.TopNdx();
    tag2.Set(dg, ndx0);
    if(tag2.Type() != ZBDDDG_OR)
      PrintD(dg, ndx0);
    else
    {
      ndx0 = tag2.TopNdx();
      PrintD(dg, ndx0);
      ndx0 = tag2.NextNdx();
      while(ndx0 != ZBDDDG_NIL)
      {
        bout.Delimit();
        PrintD(dg, ndx0);
        ndx0 = tag2.NextNdx();
      }
    }
    bout.Delimit();
    bout << "1";
    bout.Delimit();
    bout << ")";
    break;
  case ZBDDDG_LIT:
    top = tag.Func().Top();
    bout << VTable.GetName(top);
    break;
  case ZBDDDG_AND:
    bout << "AND(";
    bout.Delimit();
    ndx0 = tag.TopNdx();
    PrintD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    bout.Delimit();
    bout << ")";
    break;
  case ZBDDDG_OR:
    bout << "OR(";
    bout.Delimit();
    ndx0 = tag.TopNdx();
    PrintD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    bout.Delimit();
    bout << ")";
    break;
  case ZBDDDG_OTHER:
    bout << "[";
    bout.Delimit();
    ndx0 = tag.TopNdx();
    PrintD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    bout.Delimit();
    bout << "]";
    break;
  default:
    bout << "???";
    bout.Delimit();
    break;
  }
}

int PrintDecomp(CtoI a)
{
  if(a == CtoI_Null()) return 1;
  a = a.NonZero();
  if(a == CtoI_Null()) return 1;
  ZBDD f = a.GetZBDD();
  ZBDDDG* dg = new ZBDDDG();
  if(dg == 0) return 1;
  bddword ndx = dg->Decomp(f);
  if(ndx == ZBDDDG_NIL) { delete dg; return 1; }

  PrintD(dg, ndx);
  bout.Return();
  delete dg;
  return 0;
}

static int PrintDD_N;
static void PrintDD(ZBDDDG *, bddword);
void PrintDD(ZBDDDG* dg, bddword ndx)
{
  ZBDDDG_Tag tag, tag2;
  tag.Set(dg, ndx);
  bddword ndx0, ndx2;
  char s[20];
  int top;
  int n;
  switch(tag.Type())
  {
  case ZBDDDG_C0:
    bout << "n0 [label=0];";
    break;
  case ZBDDDG_P1:
    n = PrintDD_N++;
    sprintf(s, "n%d", n); bout << s;
    bout << " [label=OR];";
    bout.Return();

    sprintf(s, "n%d", n); bout << s;
    bout << " -> ";
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << ";";
    bout.Return();

    ndx0 = tag.TopNdx();
    tag2.Set(dg, ndx0);
    if(tag2.Type() != ZBDDDG_OR)
      PrintDD(dg, ndx0);
    else
    {
      ndx0 = tag2.TopNdx();
      PrintDD(dg, ndx0);
      ndx0 = tag2.NextNdx();
      while(ndx0 != ZBDDDG_NIL)
      {
        sprintf(s, "n%d", n); bout << s;
        bout << " -> ";
        sprintf(s, "n%d", PrintDD_N); bout << s;
        bout << ";";
        bout.Return();

        PrintDD(dg, ndx0);
        ndx0 = tag2.NextNdx();
      }
    }
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << " [label=1];";
    bout.Return();

    sprintf(s, "n%d", n); bout << s;
    bout << " -> ";
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << ";";
    bout.Return();
    PrintDD_N++;

    break;
  case ZBDDDG_LIT:
    top = tag.Func().Top();

    n = PrintDD_N++;
    sprintf(s, "n%d", n); bout << s;
    bout << " [label=";
    bout << VTable.GetName(top);
    bout << "];";
    bout.Return();

    break;
  case ZBDDDG_AND:
    n = PrintDD_N++;
    sprintf(s, "n%d", n); bout << s;
    bout << " [label=AND];";
    bout.Return();

    sprintf(s, "n%d", n); bout << s;
    bout << " -> ";
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << ";";
    bout.Return();

    ndx0 = tag.TopNdx();
    PrintDD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      sprintf(s, "n%d", n); bout << s;
      bout << " -> ";
      sprintf(s, "n%d", PrintDD_N); bout << s;
      bout << ";";
      bout.Return();

      PrintDD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    break;
  case ZBDDDG_OR:
    n = PrintDD_N++;
    sprintf(s, "n%d", n); bout << s;
    bout << " [label=OR];";
    bout.Return();

    sprintf(s, "n%d", n); bout << s;
    bout << " -> ";
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << ";";
    bout.Return();

    ndx0 = tag.TopNdx();
    PrintDD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      sprintf(s, "n%d", n); bout << s;
      bout << " -> ";
      sprintf(s, "n%d", PrintDD_N); bout << s;
      bout << ";";
      bout.Return();

      PrintDD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    break;
  case ZBDDDG_OTHER:
    n = PrintDD_N++;
    sprintf(s, "n%d", n); bout << s;
    bout << " [label=OTHER];";
    bout.Return();

    sprintf(s, "n%d", n); bout << s;
    bout << " -> ";
    sprintf(s, "n%d", PrintDD_N); bout << s;
    bout << ";";
    bout.Return();

    ndx0 = tag.TopNdx();
    PrintDD(dg, ndx0);
    ndx0 = tag.NextNdx();
    while(ndx0 != ZBDDDG_NIL)
    {
      sprintf(s, "n%d", n); bout << s;
      bout << " -> ";
      sprintf(s, "n%d", PrintDD_N); bout << s;
      bout << ";";
      bout.Return();

      PrintDD(dg, ndx0);
      ndx0 = tag.NextNdx();
    }
    break;
  default:
    bout << "???";
    break;
  }
}

int PrintDecompDot(CtoI a)
{
  if(a == CtoI_Null()) return 1;
  a = a.NonZero();
  if(a == CtoI_Null()) return 1;
  ZBDD f = a.GetZBDD();
  ZBDDDG* dg = new ZBDDDG();
  if(dg == 0) return 1;
  bddword ndx = dg->Decomp(f);
  if(ndx == ZBDDDG_NIL) { delete dg; return 1; }

  bout << "digraph G {";
  bout.Return();
  PrintDD_N = 0;
  PrintDD(dg, ndx);
  bout << "}";
  bout.Return();
  delete dg;
  return 0;
}

