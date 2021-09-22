// BEM-II Print part (SAPPORO-1.31)
// Shin-ichi MINATO (May 14, 2021)

#include <cstdio>
#include <iostream>
#include <cstring>
#include "BtoI.h"
#include "BDDDG.h"
#include "bemII.h"
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

void BOut::Return()
{
  _column = 0;
  cout << "\n";
  cout.flush();
}

BOut bout;

static BDD ISOP0(BDD, BDD);
BDD ISOP0(BDD s, BDD r)
{
  if(s == -1) return -1;
  if(r == -1) return -1;
  if(r == 1) return 0;

  int top = s.Top();
  int rtop = r.Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(rtop)) top = rtop;

  if(top == 0)
  {
    if(P_OR != 0) bout << " |";
    P_OR = 1;
    for(int i=0; i<LitIP; i++)
    {
      if(i > 0) bout << " &";
      int var = IPlist[i];
      bout.Delimit();
      if(var >= 0)
        bout << VTable.GetName(var);
      else
        bout << "!" << VTable.GetName(-var);
    }
    return s;
  }

  BDD s0 = s.At0(top);
  BDD r0 = r.At0(top);
  BDD s1 = s.At1(top);
  BDD r1 = r.At1(top);

  IPlist[LitIP++] = top;
  BDD p1 = ISOP0(s1, r1 | s0);
  if(p1 == -1) return p1;

  IPlist[LitIP-1] = -top;
  BDD p0 = ISOP0(s0, r0 | s1);
  if(p0 == -1) return p0;

  LitIP--;
  BDD sx = (s0 & s1);
  BDD px = ISOP0(sx, ~sx |( (r0|p0) & (r1|p1) ));

  BDD x = BDDvar(top);
  return (~x & p0)|(x & p1)| px;
}

static int ISOP(BDD);
int ISOP(BDD f)
{
  if(f == -1) return 1;
  if(f == 0) bout << " 0";
  else if(f == 1) bout << " 1";
  else
  {
    int N_IP = VTable.Used();
    IPlist = new int[N_IP];
    LitIP = 0;
    P_OR = 0;
    BDD cov = ISOP0(f, ~f);
    delete[] IPlist;
    if(cov == -1) return 1;
  }
  bout.Return();
  return 0;
}

static int XISOP(BDD);
int XISOP(BDD f)
{
  if(f == -1) return 1;
  if(f == 0)
  {
    bout << " 0";
    bout.Return();
    return 0;
  }
  if(f == 1)
  {
    bout << " 1";
    bout.Return();
    return 0;
  }
  int paren = 0;
  int N_IP = VTable.Used();
  for(int i=0; i<N_IP; i++)
  {
    int var = i + BDDV_SysVarTop + 1;
    BDD f0 = f.At0(var);
    if(f0 == -1) return 1;
    if(f == f0) continue;
    BDD fx = BDDvar(var) ^ f0;
    if(fx == -1) return 1;
    if(f != fx) continue;
    bout.Delimit();
    if(f0 == 0)
    {
      bout << VTable.GetName(var);
      bout.Return();
      return 0;
    }
    if(f0 == 1)
    {
      bout << "!" << VTable.GetName(var);
      bout.Return();
      return 0;
    }
    bout << VTable.GetName(var) << " ^";
    f = f0;
    paren = 1;
  }
  if(paren != 0)
  {
    bout.Delimit();
    bout << "(";
  }
  IPlist = new int[N_IP];
  LitIP = 0;
  P_OR = 0;
  BDD cov = ISOP0(f, ~f);
  delete[] IPlist;
  if(cov == -1) return 1;
  if(paren != 0)
  {
    bout << " )";
  }
  bout.Return();
  return 0;
}

int PutNum(BtoI v, int base)
{
  int ovf = 0;
  if(v.Top() > 0)
  {
    v = v.UpperBound();
    ovf = 1;
  }
  int alen = v.Len() / 3 + 12;
  char* s = new char[alen];
  int err;
  if(base == 16) err = v.StrNum16(s);
  else err = v.StrNum10(s);
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
	
int PutList(BtoI v, int xor_, int base)
{
  if(v == BtoI(BDD(-1))) return 1;
  if(base != 0 && v.Top() == 0)
  {
    if(PutNum(v, base) == 1) return 1;
    bout.Return();
    return 0;
  }
  BDD sign = v.GetSignBDD();
  int err;
  if(sign == 0)
  {
    if(v.Len() <= 2)
    {
      if(xor_ == 0) return ISOP(v.GetBDD(0));
      else return XISOP(v.GetBDD(0));
    }
  }
  else
  {
    bout << " +-:";
    if(xor_ == 0) err = ISOP(sign);
    else err = XISOP(sign);
    if(err == 1) return 1;
  }
  for(int i=v.Len()-2; i>=0; i--)
  {
    char a[16];
    sprintf(a, "%3d:", i);
    bout << a;
    if(xor_ == 0) err = ISOP(v.GetBDD(i));
    else err = XISOP(v.GetBDD(i));
    if(err == 1) return 1;
  }
  return 0;
}

void PutCode(int num, int digit) //  num < 8, digit <=3
{
  for(int i=3; i>=0; i--)
    if(i >= digit) bout << " ";
    else if((num & (1 << i)) == 0) bout << "0";
    else bout << "1";
}

int MapVar[6];

int Map(BtoI v, int dim, int base)
{
  if(v == BtoI(BDD(-1))) return 1;
  int x, y;
  switch(dim)
  {
  case 0:
    if(PutNum(v, base) == 1) return 1;
    bout.Return();
    return 0;
  case 1:
    bout << " " << VTable.GetName(MapVar[0]);
    bout.Return();
    PutCode(0, 1);
    bout << " |";
    if(PutNum(v.At0(MapVar[0]), base) == 1) return 1;
    bout.Return();
    PutCode(1, 1);
    bout << " |";
    if(PutNum(v.At1(MapVar[0]), base) == 1) return 1;
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
  default:
    return 0;
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
    int a = i ^ (i >> 1);
    bout << "  ";
    PutCode(a, x);
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
    int b = j ^ (j >> 1);
    PutCode(b, y);
    b <<= x;
    for(int i=0; i<mx; i++)
    {
      if(i == 0 || i == 4) bout << " |";
      int a = b | (i ^ (i >> 1));
      BtoI vx = v;
      for(int k=0; k<dim; k++)
        if((a & (1 << (dim-k-1))) == 0)
          vx = vx.At0(MapVar[k]);
      	else	vx = vx.At1(MapVar[k]);
      if(PutNum(vx, base) == 1) return 1;
    }
    bout.Return();
  }
  return 0;
}
			
int MapAll(BtoI v, int base)
{
  int i=0;
  for(int j=0; j<VTable.Used(); j++)
  {
    MapVar[i++] = j + BDDV_SysVarTop + 1;
    if(i == 6) break;
  }
  return Map(v, i, base);
}

int MapSel(BtoI v, int base)
{
  int i=0;
  for(int j=0; j<VTable.Used(); j++)
  {
    int var = j + BDDV_SysVarTop + 1;
    if(v == BtoI(BDD(-1))) return 1;
    if(v == v.At0(var)) continue;
    MapVar[i++] = var;
    if(i == 6) break;
  }
  return Map(v, i, base);
}

static BtoI CurBtoI;
static int PC(BDD, BtoI, int, int);
int PC(BDD cond, BtoI num, int digit, int base)
{
  if(cond == -1) return 1;
  if(num == BtoI(BDD(-1))) return 1;
  if(cond == 0) return 0;
  if(digit == 0)
  {
    int alen = num.Len() / 3 + 10;
    char* s = new char[alen];
    int err;
    if(base == 16) err = num.StrNum16(s);
    else err = num.StrNum10(s);
    if(err == 1)
    {
      delete[] s;
      return 1;
    }
    int len = strlen(s);
    for(int i=0; i<6-len; i++) bout << " ";
    bout << s;
    delete[] s;
    if(cond == 1) bout.Return();
    else
    {
      bout << ": ";
      if(XISOP(cond) == 1) return 1;
    }
    return 0;
  }
  digit--;
  BDD f = CurBtoI.GetBDD(digit);
  BDD g = cond & f;
  if(PC(g, num + (BtoI(1)<<digit), digit, base) == 1) return 1;
  g = cond & ~f;
  return PC(g, num, digit, base);
}

int PutCase(BtoI v, int base)
{
  if(v == BtoI(BDD(-1))) return 1;
  CurBtoI = v;
  int len = v.Len();
  BDD sign = v.GetSignBDD();
  if(PC(~sign, 0, len-1, base) == 1) return 1;
  return PC(sign, BtoI(-1)<<(len-1), len-1, base);
}

static int ASOP0(BtoI);
int ASOP0(BtoI v)
{
  if(v == BtoI(BDD(-1))) return 1;
  int top = v.Top();
  if(top == 0)
  {
    if(v == 0) return 0;

    BDD sign = v.GetSignBDD();
    if(sign != 0) bout << " -";
    else if(P_OR != 0) bout << " +";
    P_OR = 1;
    v = BtoI_ITE(sign, -v, v);
  
    int alen = v.Len() / 3 + 10;
    char* s = new char[alen];
    if(v.StrNum10(s) == 1)
    {
      delete[] s;
      return 1;
    }
  
    if(LitIP == 0)
    {
      bout.Delimit();
      bout << s;
      return 0;
    }
  
    if(v != BtoI(1))
    {
      bout.Delimit();
      bout << s << " *";
    }
  
    for(int i=0; i<LitIP; i++)
    {
      if(i > 0) bout << " *";
      int var = IPlist[i];
      bout.Delimit();
      bout << VTable.GetName(var);
    }
    return 0;
  }

  BtoI v0 = v.At0(top);
  BtoI v1 = v.At1(top);
  
  IPlist[LitIP++] = top;
  if(ASOP0(v1 - v0) == 1) return 1;

  LitIP--;
  return ASOP0(v0);
}

int PutASOP(BtoI v)
{
  if(v == BtoI(BDD(-1))) return 1;
  if(v.Top() == 0)
  {
    if(PutNum(v, 10) == 1) return 1;
    bout.Return();
    return 0;
  }
  int N_IP = VTable.Used();
  IPlist = new int[N_IP];
  LitIP = 0;
  P_OR = 0;
  int err = ASOP0(v);
  delete[] IPlist;
  if(err == 1) return 1;
  bout.Return();
  return 0;
}

#ifndef B_STATIC
int Plot(BtoI v, int inv)
{
  if(v == BtoI(BDD(-1))) return 1;
  BDD sign = v.GetSignBDD();
  BDDV v2 = v.GetMetaBDDV();
  if(sign == 0)
  {
    int len = v2.Len();
    if(len <= 2) v2 = v2.Part(0, 1);
    else v2 = v2.Part(0, len-1);
  }
  if(v2 == BDDV(-1)) return 1;
  if(inv == 1) v2.XPrint();
  else v2.XPrint0();
  return 0;
}
#endif

static void PrintD(BDDDG *, bddword);
void PrintD(BDDDG* dg, bddword idx)
{
  if(BDDDG_Inv(idx)) bout << "!";
  BDDDG_Tag tag;
  tag.Set(dg, idx);
  bddword idx0;
  int top;
  switch(tag.Type())
  {
  case BDDDG_C1:
    bout << "1";
    break;
  case BDDDG_LIT:
    top = tag.Func().Top();
    bout << VTable.GetName(top);
    break;
  case BDDDG_OR:
    bout << "OR(";
    bout.Delimit();
    idx0 = tag.TopIdx();
    PrintD(dg, idx0);
    idx0 = tag.NextIdx();
    while(idx0 != BDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, idx0);
      idx0 = tag.NextIdx();
    }
    bout.Delimit();
    bout << ")";
    break;
  case BDDDG_XOR:
    bout << "XOR(";
    bout.Delimit();
    idx0 = tag.TopIdx();
    PrintD(dg, idx0);
    idx0 = tag.NextIdx();
    while(idx0 != BDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, idx0);
      idx0 = tag.NextIdx();
    }
    bout.Delimit();
    bout << ")";
    break;
  case BDDDG_OTHER:
    bout << "[";
    bout.Delimit();
    idx0 = tag.TopIdx();
    PrintD(dg, idx0);
    idx0 = tag.NextIdx();
    while(idx0 != BDDDG_NIL)
    {
      bout.Delimit();
      PrintD(dg, idx0);
      idx0 = tag.NextIdx();
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

int PrintDecomp(BtoI a)
{
  if(a == BtoI(BDD(-1))) return 1;
  a = BtoI_NE(a, 0);
  if(a == BtoI(BDD(-1))) return 1;
  BDD f = a.GetBDD(0);
  BDDDG* dg = new BDDDG();
  if(dg == 0) return 1;
  bddword idx = dg->Decomp(f);
  if(idx == BDDDG_NIL) { delete dg; return 1; }

  PrintD(dg, idx);
  bout.Return();
  delete dg;
  return 0;
}

/*
int PutV(BtoI v)
{
	if(v == BtoI(BDD(-1))) return 1;
	BDDV v2 = v.GetMetaBDDV();
	int plus = 0;
	if(v.GetSignBDD() == 0)
	{
		plus = 1;
		int len = v2.Len();
		if(len <= 2) v2 = v2.Part(0, 1);
		else v2 = v2.Part(0, len-1);
	}
	if(v2 == BDDV(-1)) return 1;

	FSM* M = FSM_New();
	if(FSM_SetName(M, "BemII")== 1) return 1;
	int N_IP = VTable.Used();
	for(int i=0; i< N_IP; i++)
	{
		FSMcell* C = FSM_NewCell(M, FSM_IN);
		if(FSMcell_SetName(C, VTable.GetName(N_IP-i))== 1)
			return 1;
	}
	int N_OP = v2.Len();
	for(int i=0; i<N_OP; i++)
	{
		FSMcell* C = FSM_NewCell(M, FSM_OUT);
		char a[32];
		if(i == N_OP-1)
		{
			if(N_OP == 1) strcpy(a, "F");
			else if(plus == 1) sprintf(a, "F%d", i);
			else strcpy(a, "Fs");
		}
		else sprintf(a, "F%d", i);
		if(FSMcell_SetName(C, a)== 1) return 1;
		C = FSM_NewCell(M, FSM_DC);
		if(i == N_OP-1)
		{
			if(N_OP == 1) strcpy(a, "F_dc");
			else if(plus == 1) sprintf(a, "F%d_dc", i);
			else strcpy(a, "Fs_dc");
		}
		else sprintf(a, "F%d_dc", i);
		if(FSMcell_SetName(C, a)== 1) return 1;
	}

	FSMBDD B;
	B.IN = N_IP;
	if(B.IN == 0) B.IN++;
	B.OUT = N_OP;
	B.FF = 0;
	B.TM = 0;
	B.FUNC = v2 || BDDV(0, N_OP);
	B.INIT = 0;
	B.ORDER = ORD_Descent(B.IN);

	FSM_ImportBDD(M, B);
	FSM_PrintV(M);
	FSM_Delete(M);

	return 0;
}

*/
