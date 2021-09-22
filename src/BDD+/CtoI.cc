/******************************************
 * Combination-to-Integer function class  *
 * (SAPPORO-1.71) - Body                  *
 * (C) Shin-ichi MINATO  (Dec. 15, 2015)  *
 ******************************************/

#include "CtoI.h"

using std::cout;

static const char BC_CtoI_MULT = 40;
static const char BC_CtoI_DIV =  41;
static const char BC_CtoI_TV =  42;
static const char BC_CtoI_TVI =  43;

static const char BC_CtoI_RI =  44;
static const char BC_CtoI_FPA =  45;
static const char BC_CtoI_FPAV =  46;
static const char BC_CtoI_FPM =  47;
static const char BC_CtoI_FPC =  48;
static const char BC_CtoI_MEET =  49;

//----------- Macros for operation cache -----------
#define CtoI_CACHE_CHK_RETURN(op, fx, gx) \
  { ZBDD z = BDD_CacheZBDD(op, fx, gx); \
    if(z != -1) return CtoI(z); \
    BDD_RECUR_INC; }

#define CtoI_CACHE_ENT_RETURN(op, fx, gx, h) \
  { BDD_RECUR_DEC; \
    if(h != CtoI_Null()) BDD_CacheEnt(op, fx, gx, h._zbdd.GetID()); \
    return h; }

//-------------- Public Methods -----------
CtoI::CtoI(int n)
{
  if(n == 0) _zbdd = 0;
  else if(n == 1) _zbdd = 1;
  else if(n < 0)
  {
    if((n = -n) < 0) BDDerr("CtoI::CtoI(): overflow.", n);
    *this = -CtoI(n);
  }
  else
  {
    *this = 0;
    CtoI a = 1;
    while(n)
    {
      if(n & 1) *this += a;
      a += a;
      n >>= 1;
    }
  }
}

int CtoI::TopItem() const
{
  int v = Top();
  if(BDD_LevOfVar(v) <= BDD_TopLev()) return v;
  int v0 = Factor0(v).TopItem();
  int v1 = Factor1(v).TopItem();
  return (BDD_LevOfVar(v0) > BDD_LevOfVar(v1))? v0: v1;
}

int CtoI::TopDigit() const
{
  int v = Top();
  if(BDD_LevOfVar(v) <= BDD_TopLev()) return 0;
  int d0 = Factor0(v).TopDigit();
  int d1 = Factor1(v).TopDigit() + (1 << (BDDV_SysVarTop - v));
  return (d0 > d1)? d0: d1;
}

CtoI CtoI::FilterThen(const CtoI& a) const
{
  if(*this == 0) return 0;
  if(*this == CtoI_Null()) return CtoI_Null();
  CtoI aa = a.IsBool()? a: a.NonZero();
  if(aa == 0) return 0;
  if(IsBool()) return CtoI_Intsec(*this, aa);
  int v = Top();
  return CtoI_Union(Factor0(v).FilterThen(aa),
                 Factor1(v).FilterThen(aa).AffixVar(v));
}

CtoI CtoI::FilterElse(const CtoI& a) const
{
  if(*this == 0) return 0;
  if(*this == CtoI_Null()) return CtoI_Null();
  CtoI aa = a.IsBool()? a: a.NonZero();
  if(aa == 0) return *this;
  if(IsBool()) return CtoI_Diff(*this, aa);
  int v = Top();
  return CtoI_Union(Factor0(v).FilterElse(aa),
                 Factor1(v).FilterElse(aa).AffixVar(v));
}

CtoI CtoI::FilterRestrict(const CtoI& a) const
{
  CtoI th = IsBool()? *this: NonZero();
  CtoI aa = a.IsBool()? a: a.NonZero();
  return FilterThen(CtoI(th._zbdd.Restrict(aa._zbdd)));
}

CtoI CtoI::FilterPermit(const CtoI& a) const
{
  CtoI th = IsBool()? *this: NonZero();
  CtoI aa = a.IsBool()? a: a.NonZero();
  return FilterThen(CtoI(th._zbdd.Permit(aa._zbdd)));
}

CtoI CtoI::FilterPermitSym(int n) const
{
  CtoI th = IsBool()? *this: NonZero();
  return FilterThen(CtoI(th._zbdd.PermitSym(n)));
}

CtoI CtoI::NonZero() const
{
  if(IsBool()) return *this;
  int v = Top();
  return CtoI_Union(Factor0(v).NonZero(), Factor1(v).NonZero());
}

CtoI CtoI::ConstTerm() const
{
  if(IsBool()) return CtoI_Intsec(*this, 1);
  int v = Top();
  return CtoI_Union(Factor0(v).ConstTerm(),
         Factor1(v).ConstTerm().AffixVar(v));
}

CtoI CtoI::Digit(int index) const
{
  if(index < 0) BDDerr("CtoI::Digit(): invalid index.", index);
  CtoI a = *this;
  for(int i=0; i<BDDV_SysVarTop; i++)
  {
    if(index == 0) break;
    if(index & 1) a = a.Factor1(BDDV_SysVarTop - i);
    else a = a.Factor0(BDDV_SysVarTop - i);
    if(a == CtoI_Null()) break;
    index >>= 1;
  }
  while(! a.IsBool()) a = a.Factor0(a.Top());
  return a;
}

CtoI CtoI::EQ_Const(const CtoI& a) const
{
  return CtoI_Diff(NonZero(), NE_Const(a));
}

CtoI CtoI::NE_Const(const CtoI& a) const
{
  if(a == 0) return CtoI_NE(*this, 0);
  return CtoI_NE(*this, a * NonZero());
}

CtoI CtoI::GT_Const(const CtoI& a) const
{
  if(a == 0) return CtoI_GT(*this, 0);
  return CtoI_GT(*this, a * NonZero());
}

CtoI CtoI::GE_Const(const CtoI& a) const
{
  if(a == 0) return CtoI_GE(*this, 0);
  return CtoI_GE(*this, a * NonZero());
}

CtoI CtoI::LT_Const(const CtoI& a) const
{
  if(a == 0) return CtoI_GT(0, *this);
  return CtoI_GT(a * NonZero(), *this);
}

CtoI CtoI::LE_Const(const CtoI& a) const
{
  if(a == 0) return CtoI_GE(0, *this);
  return CtoI_GE(a * NonZero(), *this);
}

CtoI CtoI::MaxVal() const
{
  if(*this == CtoI_Null()) return 0;
  CtoI a = *this;
  CtoI cond = NonZero();
  CtoI c = 0;
  while(a != 0)
  {
    int td = a.TopDigit();
    CtoI d = Digit(td);
    if(td & 1)
    {
      CtoI cond0 = CtoI_Diff(cond, d);
      if(cond0 != 0) cond = cond0;
      else c = CtoI_Union(c, CtoI(1).ShiftDigit(td));
    }
    else
    {
      CtoI cond0 = CtoI_Intsec(cond, d);
      if(cond0 != 0)
      {
        cond = cond0;
	c = CtoI_Union(c, CtoI(1).ShiftDigit(td));
      }
    }
    if(td == 0) break;
    a = CtoI_Diff(a, d.ShiftDigit(td));
  }
  return c;
}

CtoI CtoI::MinVal() const { return - (- *this).MaxVal(); }

CtoI CtoI::TimesSysVar(int v) const
{
  if(v > BDDV_SysVarTop || v <= 0)
    BDDerr("CtoI::TimesSysVar(): invalid var ID.", v);
  if(*this == 0) return *this;
  if(*this == CtoI_Null()) return *this;
  CtoI a0 = Factor0(v);
  CtoI a1 = Factor1(v);
  if(a1 == 0) return a0.AffixVar(v);
  return CtoI_Union(a0.AffixVar(v), a1.TimesSysVar(v-1));
}

CtoI CtoI::DivBySysVar(int v) const
{
  if(v > BDDV_SysVarTop || v <= 0)
    BDDerr("CtoI::DivBySysVar(): invalid var ID.", v);
  if(*this == 0) return *this;
  if(*this == CtoI_Null()) return *this;
  CtoI a0 = Factor0(v);
  CtoI a1 = Factor1(v);
  if(a0.IsBool() || v == 1) return a1;
  return CtoI_Union(a1, a0.AffixVar(v).DivBySysVar(v-1));
}

CtoI CtoI::ShiftDigit(int pow) const
{
  CtoI a = *this;
  int v = BDDV_SysVarTop;
  int i = (pow >= 0)? pow: -pow; 
  while(i)
  {
    if(i & 1)
    {
      if(pow > 0) a = a.TimesSysVar(v);
      else a = a.DivBySysVar(v);
    }
    i >>= 1;
    v--;
  }
  return a;
}

bddword CtoI::Size() const { return _zbdd.Size(); }

int CtoI::GetInt() const
{
  if(*this == CtoI_Null()) return 0;
  if(IsBool()) return (CtoI_Intsec(*this, 1)== 0)? 0: 1;
  int v = Top();
  CtoI a;
  if((a=Factor0(v)) == CtoI_Null()) return 0;
  int n = a.GetInt();
  if((a=Factor1(v)) == CtoI_Null()) return 0;
  if(v == BDDV_SysVarTop) return n - (a.GetInt() << 1);
  return n + (a.GetInt() << (1<< (BDDV_SysVarTop - v)));
}

int CtoI::StrNum10(char* s) const
{
  if(*this == CtoI_Null()) { sprintf(s, "0"); return 1; }
  if(! IsConst()) return FilterThen(1).StrNum10(s);
  int td = TopDigit();
  if(td & 1)
  {
    if((- *this).StrNum10(s)) return 1;
    int len = strlen(s);
    for(int i=len; i>=0; i--) s[i+1] = s[i];
    s[0] = '-';
    return 0;
  }

  CtoI a = *this;
  int i=0;
  char s0[12];
  if(td >= 20)
  {
    const int width = 9;
    CtoI shift = 1000000000; // 10 ** width 
    while(1)
    {
      CtoI p = a / shift;
      if(p == 0) break;
      CtoI q = a - p * shift;
      if(q == CtoI_Null()) { sprintf(s, "0"); return 1; }
      sprintf(s0, "%09d", q.GetInt());
      for(int j=i-1; j>=0; j--) s[j+width] = s[j];
      for(int j=0; j<width; j++) s[j] = s0[j];
      i += width;
      a = p;
    }
  }
  sprintf(s0, "%d", a.GetInt());
  int len = strlen(s0);
  for(int j=i-1; j>=0; j--) s[j+len] = s[j];
  for(int j=0; j<len; j++) s[j] = s0[j];
  i += len;
  s[i] = 0;
  return 0;
}

int CtoI::StrNum16(char* s) const
{
  if(*this == CtoI_Null()) { sprintf(s, "0"); return 1; }
  if(! IsConst()) return FilterThen(1).StrNum16(s);
  int td = TopDigit();
  if(td & 1)
  {
    if((- *this).StrNum16(s)) return 1;
    int len = strlen(s);
    for(int i=len; i>=0; i--) s[i+1] = s[i];
    s[0] = '-';
    return 0;
  }

  CtoI a = *this;
  int i=0;
  char s0[12];
  if(td >= 20)
  {
    const int width = 6;
    CtoI shift = 0x1000000; // 0x10 ** width 
    while(1)
    {
      CtoI p = a / shift;
      if(p == 0) break;
      CtoI q = a - p * shift;
      if(q == CtoI_Null()) { sprintf(s, "0"); return 1; }
      sprintf(s0, "%06X", q.GetInt());
      for(int j=i-1; j>=0; j--) s[j+width] = s[j];
      for(int j=0; j<width; j++) s[j] = s0[j];
      i += width;
      a = p;
    }
  }
  sprintf(s0, "%X", a.GetInt());
  int len = strlen(s0);
  for(int j=i-1; j>=0; j--) s[j+len] = s[j];
  for(int j=0; j<len; j++) s[j] = s0[j];
  i += len;
  s[i] = 0;
  return 0;
}

static int Depth;
static int* S_Var;
static int PFflag;
static int PF(CtoI);
static int PF(CtoI a)
{
  if(a.IsConst())
  {
    if(a.TopDigit() & 1) { cout << " -"; a = -a; }
    else if(PFflag == 1) cout << " +";

    PFflag = 1;
    int c1 = (a != 1);
    if(c1 || Depth == 0)
    {
      char s[80];
      a.StrNum10(s);
      cout << " " << s;
    }
    for(int i=0; i<Depth; i++) cout << " v" << S_Var[i];
    cout.flush();
    return 0;
  }

  int v = a.TopItem();
  CtoI b = a.Factor1(v);
  if(b == CtoI_Null()) return 1;
  S_Var[Depth] = v;
  Depth++;
  if(PF(b) == 1) return 1;

  Depth--;
  b = a.Factor0(v);
  if(b == 0) return 0;
  if(b == CtoI_Null()) return 1;
  return PF(b);
}

int CtoI::PutForm() const
{
  if(*this == CtoI_Null()) return 1;
  
  if(*this == 0) cout << " 0";
  else
  {
    int v = TopItem();
    Depth = 0;
    S_Var = new int[v];
    PFflag = 0;
    int err = PF(*this);
    delete[] S_Var;
    if(err == 1)
    {
      cout << "...\n";
      cout.flush();
      return 1;
    }
  }
  cout << "\n";
  cout.flush();
  return 0;
}

void CtoI::Print() const { _zbdd.Print(); }

CtoI CtoI::CountTerms() const
{
  if(IsBool()) return TotalVal();
  return NonZero().TotalVal();
}

CtoI CtoI::TotalVal() const
{
  if(*this == CtoI_Null()) return *this;
  int top = Top();
  if(top == 0) return *this;

  bddword x = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_TV, x, 0);

  CtoI c = Factor0(top).TotalVal();
  if(c != CtoI_Null())
  {
    if(IsBool())
      c += Factor1(top).TotalVal();
    else
      c += Factor1(top).TotalVal().ShiftDigit(1<<(BDDV_SysVarTop - top));
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_TV, x, 0, c);
}

CtoI CtoI::TotalValItems() const
{
  if(*this == CtoI_Null()) return *this;
  int top = Top();
  if(top == 0) return CtoI(0);

  bddword x = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_TVI, x, 0);

  CtoI c = Factor0(top).TotalValItems();
  if(c != CtoI_Null())
  {
    CtoI a1 = Factor1(top);
    if(IsBool())
      c += a1.TotalValItems() + a1.TotalVal().AffixVar(top);
    else
      c += a1.TotalValItems().ShiftDigit(1<<(BDDV_SysVarTop - top));
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_TVI, x, 0, c);
}

CtoI CtoI::ReduceItems(const CtoI& b) const
{
  if(*this == CtoI_Null()) return *this;
  if(b == CtoI_Null()) return b;
  if(! b.IsBool()) return ReduceItems(b.NonZero());
  int atop = Top();
  int btop = b.Top();
  int top = (BDD_LevOfVar(atop) > BDD_LevOfVar(btop))? atop: btop;
  if(top == 0) return *this;

  bddword ax = _zbdd.GetID();
  bddword bx = b._zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_RI, ax, bx);

  CtoI c = CtoI(0);
  if(BDD_LevOfVar(btop) > BDD_LevOfVar(atop))
    c = ReduceItems(b.Factor0(btop));
  else if(!IsBool())
    c = Factor0(atop).ReduceItems(b)
      + Factor1(atop).ReduceItems(b).ShiftDigit(1<<(BDDV_SysVarTop - atop));
  else if(BDD_LevOfVar(btop) == BDD_LevOfVar(atop))
    c = Factor0(atop).ReduceItems(b) + Factor1(atop).ReduceItems(b);
  else
  {
    c = Factor0(atop).ReduceItems(b.Factor0(atop))
      + Factor1(atop).ReduceItems(b.Factor0(atop)).AffixVar(atop);
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_RI, ax, bx, c);
}

CtoI CtoI::FreqPatA(int Val) const
{
  if(*this == CtoI_Null()) return *this;
  if(IsConst()) return (GetInt() >= Val)? CtoI(1): CtoI(0);

  int ax = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_FPA, ax, Val);

  int top = TopItem();
  CtoI f1 = Factor1(top);

  CtoI f = f1;
  int tv = 1 << f.TopDigit();
  CtoI f2 = f1;
  if(tv -(tv>>1) < Val)
  {
    tv = 0;
    while(f != CtoI(0))
    {
      int d = f.TopDigit();
      CtoI fd = f.Digit(d);
      if(d & 1)
      {
        tv -= fd.GetZBDD().Card() << d;
        if(tv >= Val) break;
      }
      else
      {
        tv += fd.GetZBDD().Card() << d;
        if((tv>>1) >= Val) break;
      }
      f -= fd.ShiftDigit(d);
    }
    if(tv < Val) f2 = CtoI(0);
  }

  CtoI h1 = f2.FreqPatA(Val);
  CtoI h = CtoI_Union(h1, h1.AffixVar(top));
  CtoI f0 = Factor0(top);
  if(f0 != CtoI(0))
  {
    //CtoI f1v = f1.GE_Const(CtoI(Val));
    f0 = f0.FilterElse(h1);
    if(f0 != CtoI(0))
    {
      f1 = f1.FilterElse(h1);
      h = CtoI_Union(h, (f0 + f1).FreqPatA(Val));
    }
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_FPA, ax, Val, h);
}

CtoI CtoI::FreqPatA2(int Val) const
{
  if(*this == CtoI_Null()) return *this;
  if(IsConst()) return (GetInt() >= Val)? CtoI(1): CtoI(0);

  int ax = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_FPA, ax, Val);

  int top = TopItem();
  CtoI f1 = Factor1(top);

  CtoI f = f1;
  int tv = 1 << f.TopDigit();
  CtoI f2 = f1;
  if(tv -(tv>>1) < Val)
  {
    tv = 0;
    while(f != CtoI(0))
    {
      int d = f.TopDigit();
      CtoI fd = f.Digit(d);
      if(d & 1)
      {
        tv -= fd.GetZBDD().Card() << d;
        if(tv >= Val) break;
      }
      else
      {
        tv += fd.GetZBDD().Card() << d;
        if((tv>>1) >= Val) break;
      }
      f -= fd.ShiftDigit(d);
    }
    if(tv < Val) f2 = CtoI(0);
  }

  int sz = f2.Size();
  CtoI s = f2.TotalValItems().LT_Const(CtoI(Val));
  f2 = f2.ReduceItems(s);
  cout << (float) (sz - f2.Size())/sz << " ";
  cout.flush();

  CtoI h1 = f2.FreqPatA2(Val);
  CtoI h = CtoI_Union(h1, h1.AffixVar(top));
  CtoI f0 = Factor0(top);
  if(f0 != CtoI(0))
  {
    //CtoI f1v = f1.GE_Const(CtoI(Val));
    f0 = f0.FilterElse(h1);
    if(f0 != CtoI(0))
    {
      f1 = f1.FilterElse(h1);
      h = CtoI_Union(h, (f0 + f1).FreqPatA2(Val));
    }
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_FPA, ax, Val, h);
}

CtoI CtoI::FreqPatAV(int Val) const
{
  if(*this == CtoI_Null()) return *this;
  if(IsConst()) return (GetInt() >= Val)? *this: CtoI(0);

  int ax = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_FPAV, ax, Val);

  int top = TopItem();
  CtoI f1 = Factor1(top);

  CtoI f = f1;
  int tv = 1 << f.TopDigit();
  CtoI f2 = f1;
  if(tv -(tv>>1) < Val)
  {
    tv = 0;
    while(f != CtoI(0))
    {
      int d = f.TopDigit();
      CtoI fd = f.Digit(d);
      if(d & 1)
      {
        tv -= fd.GetZBDD().Card() << d;
        if(tv >= Val) break;
      }
      else
      {
        tv += fd.GetZBDD().Card() << d;
        if((tv>>1) >= Val) break;
      }
      f -= fd.ShiftDigit(d);
    }
    if(tv < Val) f2 = CtoI(0);
  }

  CtoI h1 = f2.FreqPatAV(Val);
  CtoI h = h1.AffixVar(top);

  CtoI f0 = Factor0(top);
  if(f0 == CtoI(0)) h = CtoI_Union(h, h1);
  else h = CtoI_Union(h, (f0 + f1).FreqPatAV(Val));

  CtoI_CACHE_ENT_RETURN(BC_CtoI_FPAV, ax, Val, h);
}

CtoI CtoI::FreqPatM(int Val) const
{
  if(*this == CtoI_Null()) return *this;
  if(IsConst()) return (GetInt() >= Val)? CtoI(1): CtoI(0);

  int ax = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_FPM, ax, Val);

  int top = TopItem();
  CtoI f1 = Factor1(top);

  CtoI f = f1;
  int tv = 1 << f.TopDigit();
  CtoI f2 = f1;
  if(tv -(tv>>1) < Val)
  {
    tv = 0;
    while(f != CtoI(0))
    {
      int d = f.TopDigit();
      CtoI fd = f.Digit(d);
      if(d & 1)
      {
        tv -= fd.GetZBDD().Card() << d;
        if(tv >= Val) break;
      }
      else
      {
        tv += fd.GetZBDD().Card() << d;
        if((tv>>1) >= Val) break;
      }
      f -= fd.ShiftDigit(d);
    }
    if(tv < Val) f2 = CtoI(0);
  }

  CtoI h1 = f2.FreqPatM(Val);
  CtoI h = h1.AffixVar(top);
  CtoI f0 = Factor0(top);
  if(f0 != CtoI(0))
  {
    f0 = CtoI_Diff(f0, f0.FilterPermit(h1));
    if(f0 != CtoI(0))
    {
      f1 = CtoI_Diff(f1, f1.FilterPermit(h1));
      f2 = (f0 + f1).FreqPatM(Val);
      h = CtoI_Union(h, CtoI_Diff(f2, f2.FilterPermit(h1)));
    }
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_FPM, ax, Val, h);
}

CtoI CtoI::FreqPatC(int Val) const
{
  if(*this == CtoI_Null()) return *this;
  if(IsConst()) return (GetInt() >= Val)? CtoI(1): CtoI(0);

  int ax = _zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_FPC, ax, Val);

  int top = TopItem();
  CtoI f1 = Factor1(top);

  CtoI f = f1;
  int tv = 1 << f.TopDigit();
  CtoI f2 = f1;
  if(tv -(tv>>1) < Val)
  {
    tv = 0;
    while(f != CtoI(0))
    {
      int d = f.TopDigit();
      CtoI fd = f.Digit(d);
      if(d & 1)
      {
        tv -= fd.GetZBDD().Card() << d;
        if(tv >= Val) break;
      }
      else
      {
        tv += fd.GetZBDD().Card() << d;
        if((tv>>1) >= Val) break;
      }
      f -= fd.ShiftDigit(d);
    }
    if(tv < Val) f2 = CtoI(0);
  }

  CtoI h1 = f2.FreqPatC(Val);
  CtoI h = h1.AffixVar(top);
  CtoI f0 = Factor0(top);
  if(f0 != CtoI(0))
  {
    h1 -= h1.FilterPermit(f0);
    h = CtoI_Union(h, (f0 + f1).FreqPatC(Val).FilterElse(h1));
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_FPC, ax, Val, h);
}

//----------- External functions -----------

CtoI operator+(const CtoI& a, const CtoI& b)
{
  CtoI c = CtoI_Intsec(a, b);
  CtoI s = CtoI_Diff(CtoI_Union(a, b), c);
  if(c == 0) return s;
  if(s == CtoI_Null()) return s;
  BDD_RECUR_INC;
  CtoI h = s - c.ShiftDigit(1);
  BDD_RECUR_DEC;
  return h;
}

CtoI operator-(const CtoI& a, const CtoI& b)
{
  CtoI c = CtoI_Diff(b, a);
  CtoI s = CtoI_Union(CtoI_Diff(a, b), c);
  if(c == 0) return s;
  if(s == CtoI_Null()) return s;
  BDD_RECUR_INC;
  CtoI h = s + c.ShiftDigit(1);
  BDD_RECUR_DEC;
  return h;
}

CtoI operator *(const CtoI& ac, const CtoI& bc)
{
  if(ac == 1) return bc;
  if(bc == 1) return ac;
  if(ac == CtoI_Null()) return ac;
  if(bc == CtoI_Null()) return bc;
  if(ac == 0) return 0;
  if(bc == 0) return 0;

  CtoI a = ac; CtoI b = bc;
  int atop = a.Top(); int btop = b.Top();
  if(BDD_LevOfVar(atop) < BDD_LevOfVar(btop))
  {
    a = bc; b = ac;
    atop = a.Top(); btop = b.Top();
  }

  bddword ax = a._zbdd.GetID();
  bddword bx = b._zbdd.GetID();
  if(atop == btop && ax < bx)
  {
    a = bc; b = ac;
    ax = a._zbdd.GetID(); bx = b._zbdd.GetID();
  }

  CtoI_CACHE_CHK_RETURN(BC_CtoI_MULT, ax, bx);

  CtoI a0 = a.Factor0(atop);
  CtoI a1 = a.Factor1(atop);
  CtoI c;
  if(atop != btop)
  {
    if(BDD_LevOfVar(atop) <= BDD_TopLev())
      c = CtoI_Union(a0*b, (a1*b).AffixVar(atop));
    else c = a0*b + (a1*b).TimesSysVar(atop);
  }
  else
  {
    CtoI b0 = b.Factor0(atop);
    CtoI b1 = b.Factor1(atop);
    if(BDD_LevOfVar(atop) <= BDD_TopLev())
      c = CtoI_Union(a0*b0, (a1*b0 + a0*b1 + a1*b1).AffixVar(atop));
    else if(atop > 1)
      c = a0*b0 + (a1*b0 + a0*b1).TimesSysVar(atop)
        + (a1*b1).TimesSysVar(atop - 1);
    else BDDerr("CtoI::operator*(): SysVar overflow.");
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_MULT, ax, bx, c);
}

CtoI operator /(const CtoI& ac, const CtoI& bc)
{
  if(ac == CtoI_Null()) return ac;
  if(bc == CtoI_Null()) return bc;
  if(ac == 0) return 0;
  if(ac == bc) return 1;
  if(bc == 0) BDDerr("CtoI::operator/(): Divide by zero.");

  CtoI a = ac; CtoI b = bc;
  bddword ax = a._zbdd.GetID();
  bddword bx = b._zbdd.GetID();
  CtoI_CACHE_CHK_RETURN(BC_CtoI_DIV, ax, bx);

  int v = b.TopItem();
  CtoI c;
  if(v == 0)
  {
    if(b.TopDigit() & 1) { a = -a; b = -b; }
    if(b == 1) return a;
    CtoI p = a.FilterThen(CtoI_GT(a, 0));
    if(a != p) c = (p / b) - ((p - a)/ b);
    else
    {
      int atd = a.TopDigit();
      int btd = b.TopDigit();
      if(atd < btd) return 0;
      c = a.Digit(atd);
      if(atd > btd) c = c.ShiftDigit(atd - btd - 2);
      else
      {
        CtoI cond = CtoI_GE(a, c * b);
        a = a.FilterThen(cond);
        c = c.FilterThen(cond); 
      }
      c += (a - c * b)/ b;
    }
  }
  else
  {
    CtoI a0 = a.Factor0(v);
    CtoI a1 = a.Factor1(v);
    CtoI b0 = b.Factor0(v);
    CtoI b1 = b.Factor1(v);
  
    c = a1 / b1;
    if(c != 0)
      if(b0 != 0)
      {
        CtoI c0 = a0 / b0;
        c = CtoI_ITE(CtoI_LT(c.Abs(), c0.Abs()), c, c0);
      }
  }
  
  CtoI_CACHE_ENT_RETURN(BC_CtoI_DIV, ax, bx, c);
}

CtoI CtoI_GT(const CtoI& ac, const CtoI& bc)
{
  CtoI a = ac; CtoI b = bc;
  CtoI open = CtoI_Union(a, b).NonZero();
  CtoI awin = 0;
  CtoI bwin = 0;
  CtoI awin0, bwin0;

  while(open != 0)
  {
    int atd = a.TopDigit();
    int btd = b.TopDigit();
    int td = (atd > btd)? atd: btd;
    CtoI aa = a.Digit(td);
    CtoI bb = b.Digit(td);
    if(td & 1)
    {
      awin0 = CtoI_Diff(bb, aa);
      bwin0 = CtoI_Diff(aa, bb);
    }
    else
    {
      awin0 = CtoI_Diff(aa, bb);
      bwin0 = CtoI_Diff(bb, aa);
    }
    awin = CtoI_Union(awin, awin0);
    open = CtoI_Diff(open, awin0);
    bwin = CtoI_Union(bwin, bwin0);
    open = CtoI_Diff(open, bwin0);
    if(open == CtoI_Null()) return CtoI_Null();
    if(td == 0) break;
    a = CtoI_Diff(a, aa.ShiftDigit(td)).FilterThen(open);
    b = CtoI_Diff(b, bb.ShiftDigit(td)).FilterThen(open);
  }
  return awin;
}

CtoI CtoI_GE(const CtoI& ac, const CtoI& bc)
{
  CtoI a = ac; CtoI b = bc;
  CtoI open = CtoI_Union(a, b).NonZero();
  CtoI awin = 0;
  CtoI bwin = 0;
  CtoI awin0, bwin0;

  while(open != 0)
  {
    int atd = a.TopDigit();
    int btd = b.TopDigit();
    int td = (atd > btd)? atd: btd;
    CtoI aa = a.Digit(td);
    CtoI bb = b.Digit(td);
    if(td & 1)
    {
      awin0 = CtoI_Diff(bb, aa);
      bwin0 = CtoI_Diff(aa, bb);
    }
    else
    {
      awin0 = CtoI_Diff(aa, bb);
      bwin0 = CtoI_Diff(bb, aa);
    }
    awin = CtoI_Union(awin, awin0);
    open = CtoI_Diff(open, awin0);
    bwin = CtoI_Union(bwin, bwin0);
    open = CtoI_Diff(open, bwin0);
    if(open == CtoI_Null()) return CtoI_Null();
    if(td == 0) break;
    a = CtoI_Diff(a, aa.ShiftDigit(td)).FilterThen(open);
    b = CtoI_Diff(b, bb.ShiftDigit(td)).FilterThen(open);
  }
  return CtoI_Union(awin, open);
}

static CtoI atoiX(char *, int, int);
static CtoI atoiX(char* s, int base, int blk)
{
  int times = 1;
  for(int i=0; i<blk; i++) times *= base;
  CtoI shift = times;
  int p = 0;
  int len = strlen(s);
  char *s0 = new char[blk + 1];
  CtoI a = 0;
  while(len - p > blk)
  {
    a *= shift;
    strncpy(s0, s+p, blk);
    a += CtoI((int)strtol(s0, 0, base));
    p += blk;
  }
  if(len > blk)
  {
    times = 1;
    for(int i=p; i<len; i++) times *= base;
    a *= CtoI(times);
  }
  strncpy(s0, s+p, blk);
  a += CtoI((int)strtol(s0, 0, base));
  delete[] s0;
  return a;
}
	
CtoI CtoI_atoi(char* s)
{
  if(s[0] == '0')
  {
    switch(s[1])
    {
    case 'x':
    case 'X':
      return atoiX(s+2, 16, 7);
    case 'b':
    case 'B':
      return atoiX(s+2, 2, 30);
    default:
      ;
    }
  }
  return atoiX(s, 10, 7);
}

CtoI CtoI_Meet(const CtoI& ac, const CtoI& bc)
{
  if(ac == CtoI_Null()) return ac;
  if(bc == CtoI_Null()) return bc;
  if(ac == 0) return 0;
  if(bc == 0) return 0;
  if(ac == 1 && bc == 1) return 1;

  CtoI a = ac; CtoI b = bc;
  int atop = ac.Top(); int btop = bc.Top();
  if(BDD_LevOfVar(atop) < BDD_LevOfVar(btop))
  {
    a = bc; b = ac;
    atop = a.Top(); btop = b.Top();
  }

  bddword ax = a._zbdd.GetID();
  bddword bx = b._zbdd.GetID();
  if(atop == btop && ax < bx)
  {
    a = bc; b = ac;
    ax = a._zbdd.GetID(); bx = b._zbdd.GetID();
  }

  CtoI_CACHE_CHK_RETURN(BC_CtoI_MEET, ax, bx);

  CtoI a0 = a.Factor0(atop);
  CtoI a1 = a.Factor1(atop);
  CtoI c;
  if(atop != btop)
  {
    if(a.IsBool())
      c = CtoI_Meet(a0, b) + CtoI_Meet(a1, b);
    else c = CtoI_Meet(a0, b) + CtoI_Meet(a1, b).TimesSysVar(atop);
  }
  else
  {
    CtoI b0 = b.Factor0(atop);
    CtoI b1 = b.Factor1(atop);
    if(a.IsBool())
      c = CtoI_Union(
            CtoI_Meet(a0, b0) + CtoI_Meet(a1, b0) + CtoI_Meet(a0, b1), 
	    CtoI_Meet(a1, b1).AffixVar(atop));
    else if(atop > 1)
      c = CtoI_Meet(a0, b0)
        + (CtoI_Meet(a1, b0) + CtoI_Meet(a0, b1)).TimesSysVar(atop)
        + CtoI_Meet(a1, b1).TimesSysVar(atop - 1);
    else BDDerr("CtoI_Meet(): SysVar overflow.");
  }

  CtoI_CACHE_ENT_RETURN(BC_CtoI_MEET, ax, bx, c);
}

