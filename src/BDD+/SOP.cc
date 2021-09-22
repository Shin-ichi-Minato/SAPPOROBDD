/****************************************
 * ZBDD-based SOP class (SAPPORO-1.7)   *
 * (Main part)                          *
 * (C) Shin-ichi MINATO (Sep. 4, 2015)  *
 ****************************************/

#include "SOP.h"

using std::cout;

//----------- Internal constant data for SOP -----------
static const char BC_SOP_MULT = 30;
static const char BC_SOP_DIV = 31;
static const char BC_SOP_BDD = 33;
static const char BC_ISOP1 = 34;
static const char BC_ISOP2 = 35;
static const char BC_SOP_IMPL = 36;
static const char BC_SOP_SUPPORT = 37;

//----------- Macros for operation cache -----------
#define SOP_CACHE_CHK_RETURN(op, fx, gx) \
  { ZBDD z = BDD_CacheZBDD(op, fx, gx); \
    if(z != -1) return SOP(z); \
    BDD_RECUR_INC; }

#define SOP_CACHE_ENT_RETURN(op, fx, gx, h) \
  { BDD_RECUR_DEC; \
    if(h != -1) BDD_CacheEnt(op, fx, gx, h.GetZBDD().GetID()); \
    return h; }

#define BDD_CACHE_CHK_RETURN(op, fx, gx) \
  { BDD h = BDD_CacheBDD(op, fx, gx); \
    if(h != -1) return h; \
    BDD_RECUR_INC; }

#define BDD_CACHE_ENT_RETURN(op, fx, gx, h) \
  { BDD_RECUR_DEC; \
    if(h != -1) BDD_CacheEnt(op, fx, gx, h.GetID()); \
    return h; }

//----------- External functions for SOP ----------

int SOP_NewVar() { BDD_NewVar(); return BDD_NewVar(); }

int SOP_NewVarOfLev(int lev)
{
  if(lev & 1) BDDerr("SOP_NewVarOfLev: Invalid lev.", lev);
  BDD_NewVarOfLev(lev - 1);
  return BDD_NewVarOfLev(lev);
}

SOP operator*(const SOP& pc, const SOP& qc)
{
  if(pc == -1) return -1;
  if(qc == -1) return -1;
  if(pc == 0) return 0;
  if(qc == 0) return 0;
  if(pc == 1) return qc;
  if(qc == 1) return pc;

  SOP p = pc; SOP q = qc;
  int top = p.Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(q.Top()))
  {
    p = qc; q = pc;
    top = p.Top();
  }

  bddword px = p.GetZBDD().GetID();
  bddword qx = q.GetZBDD().GetID();
  SOP_CACHE_CHK_RETURN(BC_SOP_MULT, px, qx);

  SOP p1 = p.Factor1(top);
  SOP p0 = p.Factor0(top);
  SOP pD = p.FactorD(top);
  SOP r;
  if(top != q.Top())
    r = (p1*q).And1(top) + (p0*q).And0(top) + (pD*q);
  else
  {
    SOP q1 = q.Factor1(top);
    SOP q0 = q.Factor0(top);
    SOP qD = q.FactorD(top);

    r = ((p1*q1)+(p1*qD)+(pD*q1)).And1(top)
      + ((p0*q0)+(p0*qD)+(pD*q0)).And0(top)
      + (pD*qD);
  }
		
  SOP_CACHE_ENT_RETURN(BC_SOP_MULT, px, qx, r);
}

SOP operator/(const SOP& fc, const SOP& pc)
{
  if(fc == -1) return -1;
  if(pc == -1) return -1;
  if(pc == 1) return fc;
  if(fc == pc) return 1;
  if(pc == 0) BDDerr("operator /(): Divided by zero.");

  SOP f = fc; SOP p = pc;
  int top = p.Top();
  if(BDD_LevOfVar(f.Top()) < BDD_LevOfVar(top)) return 0;
  
  bddword fx = f.GetZBDD().GetID();
  bddword px = p.GetZBDD().GetID();
  SOP_CACHE_CHK_RETURN(BC_SOP_DIV, fx, px);
  
  SOP q = -1;
  SOP p1 = p.Factor1(top);
  if(p1 != 0) q = f.Factor1(top) / p1;
  if(q != 0)
  {
    SOP p0 = p.Factor0(top);
    if(p0 != 0)
    {
      if(q == -1) q = f.Factor0(top) / p0;
      else q &= f.Factor0(top) / p0;
    }
    if(q != 0)
    {
      SOP pD = p.FactorD(top);
      if(pD != 0)
      {
        if(q == -1) q = f.FactorD(top) / pD;
        else q &= f.FactorD(top) / pD;
      }
    }
  }
		
  SOP_CACHE_ENT_RETURN(BC_SOP_DIV, fx, px, q);
}

//-------------- Class methods of SOP -----------------

SOP SOP::operator<<(int n) const
{
  if(n & 1) BDDerr("SOP::operator<<: Invalid shift.", n);
  return SOP(_zbdd << n);
}

SOP SOP::operator>>(int n) const
{
  if(n & 1) BDDerr("SOP::operator>>: Invalid shift.", n);
  return SOP(_zbdd >> n);
}

SOP SOP::And0(int v) const
{
  if(v & 1) BDDerr("SOP::And0: VarID must be even number.", v);
  ZBDD f = _zbdd.OffSet(v);
  f = f.OnSet0(v-1) + f.OffSet(v-1);
  f = f.Change(v-1);
  return SOP(f);
}

SOP SOP::And1(int v) const
{
  if(v & 1) BDDerr("SOP::And1: VarID must be even number.", v);
  ZBDD f = _zbdd.OffSet(v-1);
  f = f.OnSet0(v) + f.OffSet(v);
  f = f.Change(v);
  return SOP(f);
}

SOP SOP::Factor0(int v) const
{
  if(v & 1) BDDerr("SOP::Factor0: VarID must be even number.", v);
  ZBDD f = _zbdd.OnSet0(v-1);
  return SOP(f);
}

SOP SOP::Factor1(int v) const
{
  if(v & 1) BDDerr("SOP::Factor1: VarID must be even number.", v);
  ZBDD f = _zbdd.OnSet0(v);
  return SOP(f);
}

SOP SOP::FactorD(int v) const
{
  if(v & 1) BDDerr("SOP::FactorD: VarID must be even number.", v);
  ZBDD f = _zbdd.OffSet(v).OffSet(v-1);
  return SOP(f);
}

bddword SOP::Size() const { return _zbdd.Size(); }
bddword SOP::Cube() const { return _zbdd.Card(); }
bddword SOP::Lit() const { return _zbdd.Lit(); }

void SOP::Print() const
{
  cout << "[ " << _zbdd.GetID();
  cout << " Var:" << Top() << "(" << BDD_LevOfVar(Top()) << ")";
  cout << " Size:" << Size() << " ]\n";
  cout.flush();
}

int SOP::PrintPla() const { return SOPV(*this).PrintPla(); }

BDD SOP::GetBDD() const
{
  if(*this == -1) return -1;
  if(*this == 0) return 0;
  if(*this == 1) return 1;

  bddword sx = GetZBDD().GetID();
  BDD_CACHE_CHK_RETURN(BC_SOP_BDD, sx, 0);

  int top = Top();
  BDD x = BDDvar(top);
  SOP p1 = Factor1(top);
  SOP p0 = Factor0(top);
  SOP pD = FactorD(top);
  BDD f = (x & p1.GetBDD()) | (~x & p0.GetBDD()) | pD.GetBDD();

  BDD_CACHE_ENT_RETURN(BC_SOP_BDD, sx, 0, f);
}

SOP SOP::InvISOP() const { return SOP_ISOP(~GetBDD()); }

int SOP::IsPolyCube() const
{
  int top = Top();
  if(top == 0) return 0;
  SOP f1 = Factor1(top);
  SOP f0 = Factor0(top);
  SOP fD = FactorD(top);
  if(f1 != 0)
  {
    if(f0 != 0) return 1;
    if(fD != 0) return 1;
    return f1.IsPolyCube();
  }
  if(fD != 0) return 1;
  return f0.IsPolyCube();
}

int SOP::IsPolyLit() const
{
  int top = Top();
  if(top == 0) return 0;
  SOP fD = FactorD(top);
  if(fD != 0) return 1;
  SOP f0 = Factor0(top);
  SOP f1 = Factor1(top);
  if(f0 == 1) if(f1 == 0) return 0;
  if(f0 == 0) if(f1 == 1) return 0;
  return 1;
}

SOP SOP::Support() const
{
  if(*this == -1) return -1;
  if(*this == 0) return 0;
  if(*this == 1) return 0;

  ZBDD fz = GetZBDD().Support();
  SOP f = SOP(fz);
  int t;
  while(fz != 0)
  {
    t = fz.Top();
    fz = fz.OffSet(t);
    if(t & 1) f = f.FactorD(t+1) + SOP(1).And1(t+1);
  }

  return f;
}

SOP SOP::Divisor() const
{
  if(*this == -1) return -1;
  if(*this == 0) return 0;
  if(IsPolyCube() == 0) return 1;
  int top = Top();
  SOP f = *this;
  SOP g = Support();
  int t;
  while(g != 0)
  {
    t = g.Top();
    g = g.FactorD(t);
    SOP f0 = f.Factor0(t);
    SOP f1 = f.Factor1(t);
    if(f0.IsPolyCube() == 1) f = f0;
    else if(f1.IsPolyCube() == 1) f = f1;
  }
  return f;
}

SOP SOP::Swap(int v1, int v2) const
{
  if(v1 & 1) BDDerr("SOP::Swap: VarID must be even number.", v1);
  if(v2 & 1) BDDerr("SOP::Swap: VarID must be even number.", v2);
  ZBDD z = GetZBDD();
  z = z.Swap(v1, v2);
  z = z.Swap(v1-1, v2-1);
  return SOP(z);
}

SOP SOP::Implicants(BDD f) const
{
  if(*this == 0) return 0;
  if(f == 0) return 0;
  if(f == 1) return *this;
  if(*this == 1) return 0;
  
  bddword fx = GetZBDD().GetID();
  bddword gx = f.GetID();
  SOP_CACHE_CHK_RETURN(BC_SOP_IMPL, fx, gx);
  
  int top = Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(f.Top())) top = f.Top();

  BDD f0 = f.At0(top);
  BDD f1 = f.At1(top);
  SOP imp = Factor0(top).Implicants(f0).And0(top)
          + Factor1(top).Implicants(f1).And1(top);
  SOP pD = FactorD(top);
  if(pD != 0) imp += pD.Implicants(f0 & f1);
		
  SOP_CACHE_ENT_RETURN(BC_SOP_IMPL, fx, gx, imp);
}


//----------- External functions for SOP ----------

int SOPV_NewVar() { BDD_NewVar(); return BDD_NewVar(); }

int SOPV_NewVarOfLev(int lev)
{
  BDD_NewVarOfLev(lev - 1);
  return BDD_NewVarOfLev(lev);
}

//-------------- Class methods of SOPV -----------------

SOPV SOPV::operator<<=(int n)  { return *this = *this << n; }
SOPV SOPV::operator>>=(int n)  { return *this = *this >> n; }

SOPV SOPV::And0(int v) const
{
  if(v & 1) BDDerr("SOPV::And0: VarID must be even number.", v);
  ZBDDV f = _v.OffSet(v);
  f = f.OnSet0(v-1) + f.OffSet(v-1);
  f = f.Change(v-1);
  return SOPV(f);
}

SOPV SOPV::And1(int v) const
{
  if(v & 1) BDDerr("SOPV::And1: VarID must be even number.", v);
  ZBDDV f = _v.OffSet(v-1);
  f = f.OnSet0(v) + f.OffSet(v);
  f = f.Change(v);
  return SOPV(f);
}

SOPV SOPV::Factor0(int v) const
{
  if(v & 1) BDDerr("SOPV::Factor0: VarID must be even number.", v);
  ZBDDV f = _v.OnSet0(v-1);
  return SOPV(f);
}

SOPV SOPV::Factor1(int v) const
{
  if(v & 1) BDDerr("SOPV::Factor1: VarID must be even number.", v);
  ZBDDV f = _v.OnSet0(v);
  return SOPV(f);
}

SOPV SOPV::FactorD(int v) const
{
  if(v & 1) BDDerr("SOPV::FactorD: VarID must be even number.", v);
  ZBDDV f = _v.OffSet(v).OffSet(v-1);
  return SOPV(f);
}

bddword SOPV::Size() const { return _v.Size(); }

SOP SOPV::GetSOP(int index) const { return SOP(_v.GetZBDD(index)); }

SOPV SOPV::Swap(int v1, int v2) const
{
  if(v1 & 1) BDDerr("SOPV::Swap: VarID must be even number.", v1);
  if(v2 & 1) BDDerr("SOPV::Swap: VarID must be even number.", v2);
  ZBDDV z = GetZBDDV();
  z = z.Swap(v1, v2);
  z = z.Swap(v1-1, v2-1);
  return SOPV(z);
}

bddword SOPV::Cube() const
{
  SOPV v = *this;
  SOP sum = 0;
  while(v != SOPV())
  {
    if(v == SOPV(-1)) return 0;
    int last = v.Last();
    sum += v.GetSOP(last);
    v -= v.Mask(last);
  }
  return sum.Cube();
}

bddword SOPV::Lit() const
{
  SOPV v = *this;
  SOP sum = 0;
  while(v != SOPV())
  {
    if(v == SOPV(-1)) return 0;
    int last = v.Last();
    SOP f = v.GetSOP(last);
    sum += f;
    v -= v.Mask(last);
  }
  return sum.Lit();
}

void SOPV::Print() const
{
  int len = this -> Last() + 1;
  for(int i=0; i<len; i++)
  {
    cout << "f" << i << ": ";
    GetSOP(i).Print();
  }
  cout << "Size= " << Size() << "\n";
  cout << "Cube= " << Cube() << "\n";
  cout << "Lit=  " << Lit() << "\n\n";
  cout.flush();
}

static int Len;
static char* Array;
static int SOPV_PLA(const SOPV &, int);
static int SOPV_PLA(const SOPV& v, int tlev)
{
  if(v == SOPV(-1)) return 1;
  if(v == SOPV()) return 0;
  SOPV vv = v;
  if(tlev == 0)
  {
    cout << Array << " ";
    for(int i=0; i<Len; i++)
      if(vv.GetSOP(i) == 0) cout << "~";
      else cout << "1";
    cout << "\n";
    cout.flush();
    return 0;
  }
  else
  {
    Array[tlev/2-1] = '1';
    if(SOPV_PLA(vv.Factor1(BDD_VarOfLev(tlev)), tlev-2) == 1)
      return 1;
    Array[tlev/2-1] = '0';
    if(SOPV_PLA(vv.Factor0(BDD_VarOfLev(tlev)), tlev-2) == 1)
      return 1;
    Array[tlev/2-1] = '-';
    return SOPV_PLA(vv.FactorD(BDD_VarOfLev(tlev)), tlev-2);
  }
}

int SOPV::PrintPla() const
{
  if(*this == SOPV(-1)) return 1;
  int tlev = BDD_LevOfVar(Top());
  Len = Last() + 1;
  cout << ".i " << tlev/2 << "\n";
  cout << ".o " << Len << "\n";
  if(tlev > 0)
  {
    Array = new char[tlev/2 + 1];
    Array[tlev/2] = 0;
    int err = SOPV_PLA(*this, tlev);
    delete[] Array;
    if(err == 1) return 1;
  }
  else
  {
    for(int i=0; i<Len; i++)
      if(GetSOP(i) == 0) cout << "0";
      else cout << "1";
    cout << "\n";
  }
  cout << ".e\n";
  cout.flush();
  return 0;
}

//--------- Advanced external functions for SOP/SOPV ---------

struct BCpair
{
  BDD _f;
  SOP _cs;
  BCpair() { _f = 0; _cs = 0; }
  BCpair(const BDD& f, const SOP& cs) { _f = f; _cs = cs; }
  BCpair(const BCpair& p) { _f = p._f; _cs = p._cs; }
};

static BCpair ISOP(BDD, BDD);
BCpair ISOP(BDD s, BDD r)
{
  if(s == -1) return BCpair(-1, -1);
  if(r == -1) return BCpair(-1, -1);
  if(r == 1) return BCpair(0, 0);
  if(s == 1) return BCpair(1, 1);

  bddword sx = s.GetID();
  bddword rx = r.GetID();
  BDD f = BDD_CacheBDD(BC_ISOP1, sx, rx);
  ZBDD z = BDD_CacheZBDD(BC_ISOP2, sx, rx);
  if(f != -1) if(z != -1) return BCpair(f, SOP(z));
  BDD_RECUR_INC;

  int top = s.Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(r.Top())) top = r.Top();

  BDD s0 = s.At0(top);
  BDD r0 = r.At0(top);
  BDD s1 = s.At1(top);
  BDD r1 = r.At1(top);

  BCpair bc1 = ISOP(s1, r1 | s0);
  SOP c1 = bc1._cs.And1(top);

  BCpair bc0 = ISOP(s0, r0 | s1);
  SOP c0 = bc0._cs.And0(top);

  BDD sD = (s0 & s1);
  BCpair bcD = ISOP(sD, ~sD |( (r0|bc0._f) & (r1|bc1._f) ));
  SOP cD = bcD._cs;

  BDD x = BDDvar(top);
  f = (~x & bc0._f)|(x & bc1._f)| bcD._f;
  SOP cs = c1 + c0 + cD;

  BDD_RECUR_DEC;
  if(f == -1) return BCpair(-1, -1);
  if(cs == -1) return BCpair(-1, -1);
  BDD_CacheEnt(BC_ISOP1, sx, rx, f.GetID());
  BDD_CacheEnt(BC_ISOP2, sx, rx, cs.GetZBDD().GetID());
  return BCpair(f, cs);
}

SOP SOP_ISOP(BDD f) { return ISOP(f, ~f)._cs; }

SOP SOP_ISOP(BDD on, BDD dc)
{	
  return ISOP(on | dc, ~on |dc)._cs;
}

SOPV SOPV_ISOP(BDDV v) { return SOPV_ISOP(v, BDDV(0, v.Len())); }

SOPV SOPV_ISOP(BDDV on, BDDV dc)
{
  int len = on.Len();
  if(len != dc.Len()) BDDerr("SOPV_ISOP(): Len mismatch.");
  int top = on.Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(dc.Top())) top = dc.Top();
  SOPV csv(0, 0);
  for(int i=0; i<len; i++)
  {
    SOP cs = SOP_ISOP(on.GetBDD(i), dc.GetBDD(i));
    csv += SOPV(cs, i);
  }
  return csv;
}

SOPV SOPV_ISOP2(BDDV v) { return SOPV_ISOP2(v, BDDV(0, v.Len())); }

SOPV SOPV_ISOP2(BDDV on, BDDV dc)
{
  int len = on.Len();
  if(len != dc.Len()) BDDerr("SOPV_ISOP2(): Len mismatch,");
  int top = on.Top();
  if(BDD_LevOfVar(top) < BDD_LevOfVar(dc.Top())) top = dc.Top();
  SOPV csv;
  SOPV phase;
  for(int i=0; i<len; i++)
  {
    SOP cs1 = SOP_ISOP(on.GetBDD(i), dc.GetBDD(i));
    if(cs1 == -1) return SOPV(-1);
    SOP cs2 = SOP_ISOP(~on.GetBDD(i), dc.GetBDD(i));
    if(cs2 == -1) return SOPV(-1);
    int lit1 = (csv + SOPV(cs1, i)).Lit();
    int lit2 = (csv + SOPV(cs2, i)).Lit();
  
    if(lit1 <= lit2)
    {
      csv += SOPV(cs1, i);
      phase += SOPV(0, i+len);
    }
    else
    {
      csv += SOPV(cs2, i);
      phase += SOPV(1, i+len);
    }
  }
  return csv + phase;
}

