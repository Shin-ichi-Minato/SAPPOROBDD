/******************************************
 * Binary-to-Integer function class       *
 * (SAPPORO-1.55) - Body                  *
 * (C) Shin-ichi MINATO  (Dec. 11, 2012)  *
 ******************************************/

#include "BtoI.h"

BtoI::BtoI(int n)
{
  int k = n;
  if(k < 0)
  {
    k = -k;
    if(k < 0) BDDerr("BtoI::BtoI(): overflow.", n);
  }
  while(k != 0)
  {
    if((k & 1) != 0) _bddv = _bddv || BDD(1);
    else             _bddv = _bddv || BDD(0);
    k >>= 1;
  }
  _bddv = _bddv || BDD(0);
  if(n < 0) *this = - (*this);
}

BtoI& BtoI::operator+=(const BtoI& fv) { return *this = *this + fv; }
BtoI& BtoI::operator-=(const BtoI& fv) { return *this = *this - fv; }
BtoI& BtoI::operator*=(const BtoI& fv) { return *this = *this * fv; }
BtoI& BtoI::operator/=(const BtoI& fv) { return *this = *this / fv; }
BtoI& BtoI::operator%=(const BtoI& fv) { return *this = *this % fv; }
BtoI& BtoI::operator&=(const BtoI& fv) { return *this = *this & fv; }
BtoI& BtoI::operator|=(const BtoI& fv) { return *this = *this | fv; }
BtoI& BtoI::operator^=(const BtoI& fv) { return *this = *this ^ fv; }
BtoI& BtoI::operator<<=(const BtoI& fv) { return *this = *this << fv; }
BtoI& BtoI::operator>>=(const BtoI& fv) { return *this = *this >> fv; }

BtoI BtoI::Shift(int power) const
{
  if(power == 0) return *this;
  if(power > 0)
  {
    if(*this == 0) return *this;
    return BtoI(BDDV(0, power) || _bddv);
  }
  int p = -power;
  int len = Len();
  if(p >= len) p = len-1;
  return BtoI(_bddv.Part(p, len - p));
}
		
BtoI BtoI::operator<<(const BtoI& fv) const
{
  if(_bddv == BDDV(-1)) return *this;
  if(fv == BtoI(BDD(-1))) return fv;
  BtoI ffv = fv;
  BDD sign = ffv.GetSignBDD();
  BtoI p1 = BtoI_ITE(sign, 0, *this);
  BtoI n1 = BtoI_ITE(sign, *this, 0);
  BtoI p2 = BtoI_ITE(sign, 0, ffv);
  BtoI n2 = BtoI_ITE(sign, -ffv, 0);

  for(int i=0; i<ffv.Len(); i++)
  {
    BDD f = p2.GetBDD(i);
    if(f != 0) p1 = BtoI_ITE(f, p1.Shift(1 << i), p1);
    f = n2.GetBDD(i);
    if(f != 0) n1 = BtoI_ITE(f, n1.Shift(-(1 << i)), n1);
  }
  return p1 | n1;
}

BtoI BtoI::Sup() const
{
  BDDV fv = _bddv;
  int len = fv.Len();
  while(len > 1)
  {
    if(fv.GetBDD(len-1) != fv.GetBDD(len-2))
      break;
    fv = fv.Part(0, --len);
  }
  return BtoI(fv);
}

BtoI BtoI::UpperBound() const
{
  if(_bddv == BDDV(-1)) return *this;
  BDD d = GetSignBDD();
  BDD c0 = BDD(d==1);
  BDDV ub = c0;
  BDD cond = c0 | ~d;
  for(int i=Len()-2; i>=0; i--)
  {
    d = GetBDD(i);
    c0 = BDD((cond & d) != 0);
    if(c0 == -1) return BtoI(BDD(-1));
    ub = c0 || ub;
    cond &= ~c0 | d;
  }
  return BtoI(ub).Sup();
}

BtoI BtoI::UpperBound(const BDD& f) const
{
  if(_bddv == BDDV(-1)) return *this;
  if(f == -1) return BtoI(BDDV(-1));
  BDD d = GetSignBDD();
  BDD c0 = d.Univ(f);
  BDDV ub = c0;
  BDD cond = c0 | ~d;
  for(int i=Len()-2; i>=0; i--)
  {
    d = GetBDD(i);
    c0 = (cond & d).Exist(f);
    if(c0 == -1) return BtoI(BDD(-1));
    ub = c0 || ub;
    cond &= ~c0 | d;
  }
  return BtoI(ub).Sup();
}

int BtoI::GetInt() const
{
  if(Top() > 0)
  {
    if(_bddv == BDDV(-1)) return 0;
    return At0(Top()).GetInt();
  }
  if(GetSignBDD() != 0) return - (- *this).GetInt();
  int len = Len();
  if(len > 32) len = 32; // Assuming 32-bit int.
  int n = 0;
  for(int i=len-2; i>=0; i--)
  {
    n <<= 1;
    if(GetBDD(i) != 0) n |= 1;
  }
  return n;
}

static const char table[] = "0123456789ABCDEF";

int BtoI::StrNum10(char* s) const
{
  if(*this == BtoI(BDD(-1)))
  {
    sprintf(s, "0");
    return 1;
  }
  const int width = 9;
  BtoI fv0 = *this;
  while(fv0.Top() > 0) fv0 = fv0.At0(fv0.Top());
  int i=0;
  int sign = 0;
  if(fv0.GetSignBDD() == 1)
  {
    sign = 1;
    fv0 = -fv0;
  }
  BtoI a;
  char s0[12];
  if(fv0.Len() > 29)
  {
    BtoI fvb = BtoI(1000000000); // 10 ** width 
    while(1)
    {
      BtoI t = BtoI_GE(fv0, fvb);
      if(t == BtoI(BDD(-1)))
      {
        sprintf(s, "0");
        return 1;
      }
      if(t == BtoI(0)) break;
      a = fv0 % fvb;
      if(a == BtoI(BDD(-1)))
      {
        sprintf(s, "0");
        return 1;
      }
      sprintf(s0, "%09d", a.GetInt());
      for(int j=i-1; j>=0; j--) s[j+width] = s[j];
      for(int j=0; j<width; j++) s[j] = s0[j];
      i += width;
      fv0 /= fvb;
    }
  }
  sprintf(s0, "%d", fv0.GetInt());
  int len = strlen(s0);
  for(int j=i-1; j>=0; j--) s[j+len] = s[j];
  for(int j=0; j<len; j++) s[j] = s0[j];
  i += len;
  if(sign == 1)
  {
    for(int j=i-1; j>=0; j--) s[j+1] = s[j];
    s[0] = '-';
    i++;
  }
  s[i] = 0;
  return 0;
}

int BtoI::StrNum16(char* s) const
{
  if(*this == BtoI(BDD(-1)))
  {
    sprintf(s, "0");
    return 1;
  }
  const int width = 7;
  BtoI fv0 = *this;
  while(fv0.Top() > 0) fv0 = fv0.At0(fv0.Top());
  int i=0;
  int sign = 0;
  if(fv0.GetSignBDD() == 1)
  {
    sign = 1;
    fv0 = -fv0;
  }
  BtoI a;
  char s0[10];
  if(fv0.Len() > 27)
  {
    BtoI fvb = BtoI(1<<(width*4));
    while(1)
    {
      BtoI t = BtoI_GE(fv0, fvb);
      if(t == BtoI(BDD(-1)))
      {
        sprintf(s, "0");
        return 1;
      }
      if(t == BtoI(0)) break;
      a = fv0 & (fvb-1);
      if(a == BtoI(BDD(-1)))
      {
        sprintf(s, "0");
        return 1;
      }
      sprintf(s0, "%07X", a.GetInt());
      for(int j=i-1; j>=0; j--) s[j+width] = s[j];
      for(int j=0; j<width; j++) s[j] = s0[j];
      i += width;
      fv0 >>= width*4;
    }
  }
  sprintf(s0, "%X", fv0.GetInt());
  int len = strlen(s0);
  for(int j=i-1; j>=0; j--) s[j+len] = s[j];
  for(int j=0; j<len; j++) s[j] = s0[j];
  i += len;
  if(sign == 1)
  {
    for(int j=i-1; j>=0; j--) s[j+1] = s[j];
    s[0] = '-';
    i++;
  }
  s[i] = 0;
  return 0;
}

bddword BtoI::Size() const { return _bddv.Size(); }

void BtoI::Print() const { _bddv.Print(); }

BtoI operator+(const BtoI& a, const BtoI& b)
{
  if(a == 0) return b;
  if(b == 0) return a;
  if(a == BtoI(BDD(-1))) return a;
  if(b == BtoI(BDD(-1))) return b;
  BDD a0;
  BDD b0;
  BDD c = 0;
  BDD c0 = 0;
  BDDV fv;
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  for(int i=0; i<len; i++)
  {
    a0 = a.GetBDD(i);
    b0 = b.GetBDD(i);
    c0 = c;
    fv = fv || (a0 ^ b0 ^ c0);
    c = (a0 & b0)|(b0 & c0)|(c0 & a0);
  }
  if(c != c0) fv = fv || (a0 ^ b0 ^ c);
  return BtoI(fv).Sup();
}

BtoI operator-(const BtoI& a, const BtoI& b)
{
  if(b == 0) return a;
  if(b == BtoI(BDD(-1))) return b;
  if(a == b) return 0;
  if(a == BtoI(BDD(-1))) return a;
  BDD a0;
  BDD b0;
  BDD c = 0;
  BDD c0 = 0;
  BDDV fv;
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  for(int i=0; i<len; i++)
  {
    a0 = a.GetBDD(i);
    b0 = b.GetBDD(i);
    c0 = c;
    fv = fv || (a0 ^ b0 ^ c0);
    c = (~a0 & b0)|(b0 & c0)|(c0 & ~a0);
  }
  if(c != c0) fv = fv || (a0 ^ b0 ^ c);
  return BtoI(fv).Sup();
}

BtoI operator*(const BtoI& a, const BtoI& b)
{
  if(a == 1) return b;
  if(b == 1) return a;
  if(a == BtoI(BDD(-1))) return a;
  if(b == BtoI(BDD(-1))) return b;
  if(a == 0) return 0;
  if(b == 0) return 0;
  BDD sign = b.GetSignBDD();
  BtoI a0 = BtoI_ITE(sign, -a, a);
  BtoI b0 = BtoI_ITE(sign, -b, b);
  BtoI s = BtoI_ITE(b.GetBDD(0), a0, 0);
  while(b0 != 0)
  {
    a0 <<= 1;
    b0 >>= 1;
    s += BtoI_ITE(b0.GetBDD(0), a0, 0);
    if(s == BtoI(BDD(-1))) break;
  }
  return s;
}

BtoI operator/(const BtoI& a, const BtoI& b)
{
  if(b == 1) return a;
  if(a == BtoI(BDD(-1))) return a;
  if(b == BtoI(BDD(-1))) return b;
  if(a == 0) return 0;
  if(a == b) return 1;
  if(BtoI_EQ(b, 0) != 0) 
    BDDerr("BtoI::operator/(): Divided by 0.");
  BDD sign = a.GetSignBDD() ^ b.GetSignBDD();
  BtoI a0 = BtoI_ITE(a.GetSignBDD(), -a, a);
  if(a0 == BtoI(BDD(-1))) return a0;
  BtoI b0 = BtoI_ITE(b.GetSignBDD(), -b, b);
  if(b0 == BtoI(BDD(-1))) return b0;
  int len = a0.Len();
  b0 <<= len - 2;
  BtoI s = 0;
  for(int i=0; i<len-1; i++)
  {
    s <<= 1;
    BtoI cond = BtoI_GE(a0, b0);
    a0 = BtoI_ITE(cond, a0 - b0, a0);
    s += cond;
    b0 >>= 1;
  }
  return BtoI_ITE(sign, -s, s);
}

BtoI operator&(const BtoI& a, const BtoI& b)
{
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  BDDV fv;
  for(int i=0; i<len; i++)
    fv = fv || (a.GetBDD(i) & b.GetBDD(i));
  return BtoI(fv).Sup();
}

BtoI operator|(const BtoI& a, const BtoI& b)
{
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  BDDV fv;
  for(int i=0; i<len; i++)
    fv = fv || (a.GetBDD(i) | b.GetBDD(i));
  return BtoI(fv).Sup();
}

BtoI operator^(const BtoI& a, const BtoI& b)
{
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  BDDV fv;
  for(int i=0; i<len; i++)
    fv = fv || (a.GetBDD(i) ^ b.GetBDD(i));
  return BtoI(fv).Sup();
}

BtoI BtoI_ITE(const BDD& f, const BtoI& a, const BtoI& b)
{
  if(a == b) return a;
  int len = a.Len();
  if(len < b.Len()) len = b.Len();
  BDDV fv;
  for(int i=0; i<len; i++)
  {
    BDD g = (f & a.GetBDD(i)) | (~f & b.GetBDD(i));
    fv = fv || g;
  }
  return BtoI(fv).Sup();
}

BtoI BtoI_EQ(const BtoI& a, const BtoI& b)
{
  BtoI c = a - b;
  BDD cond = 0;
  for(int i=0; i<c.Len(); i++)
  {
    cond |= c.GetBDD(i);
    if(cond == 1) break;
  }
  return BtoI(~cond);
}

static BtoI atoi16(char *);
static BtoI atoi16(char* s)
{
  const int width = 6;
  int p = 0;
  int len = strlen(s);
  char a[width + 1];
  BtoI fv = 0;
  while(len - p > width)
  {
    fv <<= BtoI(width*4);
    strncpy(a, s+p, width);
    fv += BtoI((int)strtol(a, 0, 16));
    p += width;
  }
  if(len > width) fv <<= BtoI((len-p)*4);
  strncpy(a, s+p, width);
  fv += BtoI((int)strtol(a, 0, 16));
  return fv;
}
	
static BtoI atoi2(char *);
static BtoI atoi2(char* s)
{
  int p = 0;
  int len = strlen(s);
  BtoI fv = 0;
  while(p < len)
  {
    fv <<= BtoI(1);
    if(s[p] == '1') fv += 1;
    p++;
  }
  return fv;
}
	
static BtoI atoi10(char *);
static BtoI atoi10(char* s)
{
  const int width = 9;
  int times = 1000000000; // 10 ** width
  int p = 0;
  int len = strlen(s);
  char a[width + 1];
  BtoI fv = 0;
  while(len - p > width)
  {
    fv *= BtoI(times);
    strncpy(a, s+p, width);
    fv += BtoI((int)strtol(a, 0, 10));
    p += width;
  }
  if(len > width)
  {
    times = 1;
    for(int i=p; i<len; i++) times *= 10;
    fv *= BtoI(times);
  }
  strncpy(a, s+p, width);
  fv += BtoI((int)strtol(a, 0, 10));
  return fv;
}
	
BtoI BtoI_atoi(char* s)
{
  if(s[0] == '0')
  {
    switch(s[1])
    {
    case 'x':
    case 'X':
      return atoi16(s+2);
    case 'b':
    case 'B':
      return atoi2(s+2);
    default:
      ;
    }
  }
  return atoi10(s);
}

