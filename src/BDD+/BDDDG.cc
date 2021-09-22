/*******************************************
 * BDD - Decomposition Graph (SAPPORO-1.59)*
 * (C) Shin-ichi MINATO (Dec. 10, 2013)    *
 *******************************************/

#include "BDDDG.h"

using std::cout;
using std::cerr;

BDDDG_Tag::BDDDG_Tag()
{
  _dg = 0;
  _ndx = BDDDG_NIL;
}

int BDDDG_Tag::Set(BDDDG* dg, bddword idx)
{
  _dg = dg;
  if(idx == BDDDG_NIL) return 1;
  _ndx = BDDDG_Ndx(idx);
  if(_ndx >= _dg->_nodeUsed) return 1;
  _lkx = _dg->_nodeA[_ndx]._lkx;
  return 0;
}

bddword BDDDG_Tag::TopIdx()
{
  _lkx = _dg->_nodeA[_ndx]._lkx;
  if(_lkx == BDDDG_NIL) return BDDDG_NIL;
  return _dg->_linkA[_lkx]._idx;
}

bddword BDDDG_Tag::NextIdx()
{
  _lkx = _dg->_linkA[_lkx]._nxt;
  if(_lkx == BDDDG_NIL) return BDDDG_NIL;
  return _dg->_linkA[_lkx]._idx;
}

char BDDDG_Tag::Type()
{
  return _dg->_nodeA[_ndx]._type;
}

BDD BDDDG_Tag::Func()
{
  return _dg->_nodeA[_ndx]._f;
}

BDDDG::BDDDG()
{
  _nodeA = 0;
  _linkA = 0;
  _hashWheel = 0;
  Clear();
}

BDDDG::~BDDDG()
{
  delete[] _hashWheel;
  delete[] _linkA;
  delete[] _nodeA;
}

void BDDDG::Clear()
{
  delete[] _hashWheel;
  delete[] _linkA;
  delete[] _nodeA;
   
  _nodeSize = BDDDG_InitSize;
  _nodeA = new Node[_nodeSize];
  _nodeUsed = 0;
  _linkSize = BDDDG_InitSize;
  _linkA = new NodeLink[_linkSize];
  _linkUsed = 0;
  bddword hashSize = _nodeSize << 1;
  _hashWheel = new bddword[hashSize];
  for(bddword i=0; i<hashSize; i++) _hashWheel[i] = BDDDG_NIL;
  bddword cdx = NewNdx(1, BDDDG_C1);
  _c1 = BDDDG_PackIdx(cdx, 0);
}

bddword BDDDG::HashIndex(BDD key)
{
  bddword id0 = key.GetID();
  bddword id1 = (~key).GetID();
  bddword id =  (id0<id1)? id0: id1;
  bddword hashSize = _nodeSize << 1;
  bddword hash = (id+(id>>10)+(id>>20)) & (hashSize - 1);
  bddword i = hash;
  while(_hashWheel[i] != BDDDG_NIL)
  {
    BDD f = _nodeA[_hashWheel[i]]._f;
    if(key == f) return i;
    if(~key == f) return i;
    i++;
    i &= (hashSize -1);
  }
  return i;
}

bddword BDDDG::NewNdx(BDD f, char type)
{
  if(_nodeUsed == _nodeSize)
    if(EnlargeNode()) return BDDDG_NIL;
  bddword ndx = _nodeUsed++;
  _nodeA[ndx]._f = f;
  _nodeA[ndx]._type = type;
  bddword i = HashIndex(f);
  if(_hashWheel[i] != BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::NewNdx(): Duplicate node\n";
    exit(1);
  }
  _hashWheel[i] = ndx;
  return ndx;
}


int BDDDG::EnlargeNode()
{
  bddword oldHS = _nodeSize << 1;
  Node* oldArray = _nodeA;
  bddword* oldWheel = _hashWheel;

  _nodeSize <<= 2;
  _nodeA = new Node[_nodeSize];
  bddword hashSize = _nodeSize << 1;
  _hashWheel = new bddword[hashSize];
  if(_nodeA == 0 || _hashWheel == 0)
  {
    cerr << "<ERROR> BDDDG::EnlargeNode(): Memory overflow (";
    cerr << _nodeSize << ")\n";
    return 1;
  }
  for(bddword i=0; i<_nodeUsed; i++)
  {
    _nodeA[i]._lkx = oldArray[i]._lkx;
    _nodeA[i]._f = oldArray[i]._f;
    _nodeA[i]._type = oldArray[i]._type;
    _nodeA[i]._mark = oldArray[i]._mark;
    _nodeA[i]._ndxP = oldArray[i]._ndxP;
    _nodeA[i]._invP = oldArray[i]._invP;
  }
  for(bddword i=0; i<hashSize; i++) _hashWheel[i] = BDDDG_NIL;
  for(bddword i=0; i<oldHS; i++)
  {
    bddword ndx = oldWheel[i];
    if(ndx != BDDDG_NIL)
    {
      BDD f = _nodeA[ndx]._f;
      _hashWheel[HashIndex(f)] = ndx;
    }
  }
  delete[] oldArray;
  delete[] oldWheel;
  return 0;
}

bddword BDDDG::NewLkx(bddword idx)
{
  if(_linkUsed == _linkSize)
    if(EnlargeLink()) return BDDDG_NIL;
  bddword lkx = _linkUsed++;
  _linkA[lkx]._idx = idx;
  return lkx;
}

int BDDDG::EnlargeLink()
{
  NodeLink* oldArray = _linkA;

  _linkSize <<= 2;
  _linkA = new NodeLink[_linkSize];
  if(_linkA == 0)
  {
    cerr << "<ERROR> BDDDG::EnlargeLink(): Memory overflow (";
    cerr << _linkSize << ")\n";
    return 1;
  }
  for(bddword i=0; i<_linkUsed; i++)
  {
    _linkA[i]._idx = oldArray[i]._idx;
    _linkA[i]._nxt = oldArray[i]._nxt;
  }
  delete[] oldArray;
  return 0;
}

bddword BDDDG::ReferIdx(BDD key)
{
  bddword i = HashIndex(key);
  bddword ndx = _hashWheel[i];
  if(ndx == BDDDG_NIL) return ndx;
  return BDDDG_PackIdx(ndx, key != _nodeA[ndx]._f);
}

bddword BDDDG::NodeUsed() { return _nodeUsed; }

BDDDG::Node::Node()
{
  _lkx = BDDDG_NIL;
  _f = BDD(1);
  _type = BDDDG_C1;
  _mark = 0;
  _ndxP = BDDDG_NIL;
  _invP = 0;
}

BDDDG::Node::Node(BDD f, char type)
{
  _lkx = BDDDG_NIL;
  _f = f;
  _type = type;
  _mark = 0;
  _ndxP = BDDDG_NIL;
  _invP = 0;
}

void BDDDG::PhaseSweep(bddword idx)
{
  int fin = 0;
  int inv = 0;
  if(idx == BDDDG_NIL) return;
  Node* np = & _nodeA[BDDDG_Ndx(idx)];
  bddword lkx = np->_lkx;

  switch(np->_type)
  {
  case BDDDG_OR:
    /* Assertion check*/
    while(lkx != BDDDG_NIL)
    {
      NodeLink* lp = _linkA + lkx;
      if(!BDDDG_Inv(lp->_idx) &&
         _nodeA[BDDDG_Ndx(lp->_idx)]._type == BDDDG_OR)
      {
        cerr << "<ERROR> BDDDG::PhaseSweep(): Bad structure (OR)\n";
        exit(1);
      }
      lkx = lp->_nxt;
      fin++;
    }
    if(fin < 2)
    {
      cerr << "<ERROR> BDDDG::PhaseSweep(): Bad fan-in (OR)\n";
      exit(1);
    }
    break;

  case BDDDG_XOR:
    /* Assertion check*/
    while(lkx != BDDDG_NIL)
    {
      NodeLink* lp = _linkA + lkx;
      if(BDDDG_Inv(lp->_idx))
      {
	inv++;
	lp->_idx = BDDDG_InvReset(lp->_idx);
      }
      if(_nodeA[BDDDG_Ndx(lp->_idx)]._type == BDDDG_XOR)
      {
        cerr << "<ERROR> BDDDG::PhaseSweep(): Bad structure (XOR)\n";
        exit(1);
      }
      lkx = lp->_nxt;
      fin++;
    }
    if(inv & 1) np->_f = ~(np->_f);
    if(fin < 2)
    {
      cerr << "<ERROR> BDDDG::PhaseSweep(): Bad fan-in (XOR)\n";
      exit(1);
    }
    break;

  case BDDDG_OTHER:
    /* Assertion check*/
    while(lkx != BDDDG_NIL)
    {
      NodeLink* lp = _linkA + lkx;
      if(BDDDG_Inv(lp->_idx))
	lp->_idx = BDDDG_InvReset(lp->_idx);
      lkx = lp->_nxt;
      fin++;
    }
    if(fin < 2)
    {
      cerr << "<ERROR> BDDDG::PhaseSweep(): Bad fan-in (OTHER)\n";
      exit(1);
    }
    break;
  default:
    cerr << "<ERROR> BDDDG::PhaseSweep(): Bad node type\n";
    exit(1);
  }
}

int BDDDG::LinkNodes(bddword idx, bddword idx2)
{
  if(idx == BDDDG_NIL || idx2 == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::LinkNodes(): Null node\n";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  bddword ndx2 = BDDDG_Ndx(idx2);
  bddword lkx = NewLkx(idx2);
  if(lkx == BDDDG_NIL) return 1;
  _linkA[lkx]._nxt = _nodeA[ndx]._lkx;
  _nodeA[ndx]._lkx = lkx;

  bddword lkx2 = _linkA[lkx]._nxt;
  while(lkx2 != BDDDG_NIL)
  {
    bddword idx3 = _linkA[lkx]._idx;
    bddword idx4 = _linkA[lkx2]._idx;
    BDD f = _nodeA[BDDDG_Ndx(idx3)]._f;
    BDD f2 = _nodeA[BDDDG_Ndx(idx4)]._f;
    if(f.Top() == f2.Top())
    {
      cerr << "<ERROR> BDDDG::LinkNodes(): Same VarIndex(";
      cerr << f.Top() << ")\n";
      exit(1);
    }

    if(f.Top() < f2.Top()) break;

    _linkA[lkx]._idx = idx4;
    _linkA[lkx2]._idx = idx3;

    lkx = lkx2;
    lkx2 = _linkA[lkx2]._nxt;
  }
  return 0;
}

bddword BDDDG::Decomp(BDD f)
{
  if(f==0) return BDDDG_InvSet(_c1);
  if(f==1) return _c1;

  bddword idx = ReferIdx(f);
  if(idx != BDDDG_NIL) return idx;

  int top = f.Top();
  BDD f0 = f.At0(top);
  BDD f1 = f.At1(top);
  bddword idx0 = Decomp(f0);
  if(idx0 == BDDDG_NIL) return BDDDG_NIL;
  bddword idx1 = Decomp(f1);
  if(idx1 == BDDDG_NIL) return BDDDG_NIL;
  idx = Merge(f, idx0, idx1);

  return idx;
}

void BDDDG::MarkSweep(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::MarkSweep(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  _nodeA[ndx]._mark = 0;
  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    _nodeA[BDDDG_Ndx(_linkA[lkx]._idx)]._mark = 0;
    lkx = _linkA[lkx]._nxt;
  }
}

void BDDDG::MarkSweepR(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::MarkSweepR(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  _nodeA[ndx]._mark = 0;
  _nodeA[ndx]._ndxP = BDDDG_NIL;
  _nodeA[ndx]._invP = 0;
  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    MarkSweepR(_linkA[lkx]._idx);
    lkx = _linkA[lkx]._nxt;
  }
}

int BDDDG::Mark1(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::Mark1(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  int fin = 0;
  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    bddword idx1 = _linkA[lkx]._idx;
    if(BDDDG_Inv(idx1)) _nodeA[BDDDG_Ndx(idx1)]._mark = 2;
    else _nodeA[BDDDG_Ndx(idx1)]._mark = 1;
    fin++;
    lkx = _linkA[lkx]._nxt;
  }
  return fin;
}

void BDDDG::Mark2R(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::Mark2R(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  _nodeA[ndx]._mark++;
  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    Mark2R(_linkA[lkx]._idx);
    lkx = _linkA[lkx]._nxt;
  }
}

int BDDDG::MarkChkR(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::MarkChkR(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  if(_nodeA[ndx]._mark != 0) return 1;
  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    if(MarkChkR(_linkA[lkx]._idx)) return 1;
    lkx = _linkA[lkx]._nxt;
  }
  return 0;
}

void BDDDG::Mark3R(bddword idx)
{
  if(idx == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::Mark3R(): Bad idx";
    exit(1);
  }
  bddword ndx = BDDDG_Ndx(idx);
  if(_nodeA[ndx]._mark == 2) return;

  int cnt1 = 0;  // not decided.
  int cnt2 = 0;  // shared node.
  int cnt3 = 0;  // non-shared node.
  int cnt4 = 0;  // (possibly) partly shared node.

  bddword lkx = _nodeA[ndx]._lkx;
  while(lkx != BDDDG_NIL)
  {
    bddword idt = _linkA[lkx]._idx;
    Mark3R(idt);
    switch(_nodeA[BDDDG_Ndx(idt)]._mark)
    {
    case 1:
      cnt1++;
      break;
    case 2:
      cnt2++;
      break;
    case 3:
      cnt3++;
      break;
    case 4:
      cnt4++;
      break;
    default:
      break;
    }
    lkx = _linkA[lkx]._nxt;
  }
  if(_nodeA[ndx]._type == BDDDG_OR || _nodeA[ndx]._type == BDDDG_XOR)
  {
    if(cnt2 >= 2)
    {
      lkx = _nodeA[ndx]._lkx;
      while(lkx != BDDDG_NIL)
      {
        bddword idt = _linkA[lkx]._idx;
        _nodeA[BDDDG_Ndx(idt)]._ndxP = ndx;
        _nodeA[BDDDG_Ndx(idt)]._invP = (char) BDDDG_Inv(idt);
        lkx = _linkA[lkx]._nxt;
      }
      _nodeA[ndx]._mark = 4;
      return;
    }
  }
  
  if(cnt1 + cnt2 + cnt4 == 0 && _nodeA[ndx]._mark == 1)
    _nodeA[ndx]._mark = 3;
}

bddword BDDDG::Merge(BDD f, bddword idx0, bddword idx1)
{
  if(idx0 == BDDDG_NIL || idx1 == BDDDG_NIL)
  {
    cerr << "<ERROR> BDDDG::Merge(): Null node\n";
    exit(1);
  }
  bddword ndx0 = BDDDG_Ndx(idx0);
  bddword ndx1 = BDDDG_Ndx(idx1);

  int top = f.Top(); // (top > 0)

  // Case3-LIT ?
  if(BDDDG_InvReset(idx0) == _c1 && BDDDG_InvReset(idx1) == _c1)
  {
    bddword idx = ReferIdx(f);
    if(idx == BDDDG_NIL)
    {
      bddword ndx = NewNdx(BDDvar(top), BDDDG_LIT);
      if(ndx == BDDDG_NIL) return BDDDG_NIL;
      idx = BDDDG_PackIdx(ndx, f != BDDvar(top));
    }
    return idx;
  }

  // Case3-OR ?
  if(BDDDG_InvReset(idx1) == _c1)
  {
    bddword idx = ReferIdx(BDDvar(top));
    if(idx == BDDDG_NIL)
    {
      bddword ndx = NewNdx(BDDvar(top), BDDDG_LIT);
      if(ndx == BDDDG_NIL) return BDDDG_NIL;
      idx = BDDDG_PackIdx(ndx, 0);
    }
    bddword ndy = NewNdx(BDDDG_Inv(idx1)? ~f: f, BDDDG_OR);
    if(ndy == BDDDG_NIL) return BDDDG_NIL;
    bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx1));
    if(LinkNodes(idy, idx)) return BDDDG_NIL;
    if(_nodeA[ndx0]._type != BDDDG_OR ||
      BDDDG_Inv(idx0) != BDDDG_Inv(idx1) )
    {
      if(LinkNodes(idy, BDDDG_Inv(idx1)? BDDDG_InvAlt(idx0): idx0))
        return BDDDG_NIL;
    }
    else
    {
      bddword lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {
        if(LinkNodes(idy, _linkA[lkx]._idx)) return BDDDG_NIL;
	lkx = _linkA[lkx]._nxt;
      }
    }
    PhaseSweep(idy);
    return idy;
  }

  if(BDDDG_InvReset(idx0) == _c1)
  {
    bddword idx = ReferIdx(~BDDvar(top));
    if(idx == BDDDG_NIL)
    {
      bddword ndx = NewNdx(BDDvar(top), BDDDG_LIT);
      if(ndx == BDDDG_NIL) return BDDDG_NIL;
      idx = BDDDG_PackIdx(ndx, 1);
    }
    bddword ndy = NewNdx(BDDDG_Inv(idx0)? ~f: f, BDDDG_OR);
    if(ndy == BDDDG_NIL) return BDDDG_NIL;
    bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx0));
    if(LinkNodes(idy, idx)) return BDDDG_NIL;
    if(_nodeA[ndx1]._type != BDDDG_OR ||
      BDDDG_Inv(idx1) != BDDDG_Inv(idx0) )
    {
      if(LinkNodes(idy, BDDDG_Inv(idx0)? BDDDG_InvAlt(idx1): idx1))
        return BDDDG_NIL;
    }
    else
    {
      bddword lkx = _nodeA[ndx1]._lkx;
      while(lkx != BDDDG_NIL)
      {
        if(LinkNodes(idy, _linkA[lkx]._idx)) return BDDDG_NIL;
	lkx = _linkA[lkx]._nxt;
      }
    }
    PhaseSweep(idy);
    return idy;
  }

  // Case3-XOR ?
  if(ndx0 == ndx1)
  {
    if(idx0 == idx1) { cout << "ERROR\n"; exit(1); }

    bddword idx = ReferIdx(BDDvar(top));
    if(idx == BDDDG_NIL)
    {
      bddword ndx = NewNdx(BDDvar(top), BDDDG_LIT);
      if(ndx == BDDDG_NIL) return BDDDG_NIL;
      idx = BDDDG_PackIdx(ndx, 0);
    }

    bddword ndy = NewNdx(BDDDG_Inv(idx0)? ~f: f, BDDDG_XOR);
    if(ndy == BDDDG_NIL) return BDDDG_NIL;
    bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx0));
    if(LinkNodes(idy, idx)) return BDDDG_NIL;
    if(_nodeA[ndx0]._type != BDDDG_XOR)
    {
      if(LinkNodes(idy, BDDDG_InvReset(idx0))) return BDDDG_NIL;
    }
    else
    {
      bddword lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {
        if(LinkNodes(idy, _linkA[lkx]._idx)) return BDDDG_NIL;
	lkx = _linkA[lkx]._nxt;
      }
    }
    PhaseSweep(idy);
    return idy;
  }

  // Case1-OR ?
  if(_nodeA[ndx0]._type == BDDDG_OR && _nodeA[ndx1]._type == BDDDG_OR
    && BDDDG_Inv(idx0) == BDDDG_Inv(idx1))
  {
    int fin0 = 0;
    int fin1 = 0;
    int fin2 = 0;
  
    fin0 = Mark1(idx0);
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark == (BDDDG_Inv(idt)? 2: 1))
      {
	fin2++; 
        _nodeA[BDDDG_Ndx(idt)]._mark = 3;
      }
      lkx = _linkA[lkx]._nxt;
      fin1++;
    }

    if(fin2 > 0)
    {
      bddword idy0; 
      if(fin0 - fin2 > 1)
      {
        BDD g = 0;
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	  {
	    BDD g2 = _nodeA[BDDDG_Ndx(idt)]._f;
	    g |= BDDDG_Inv(idt)? ~g2: g2;
	  }
          lkx = _linkA[lkx]._nxt;
        }
        idy0 = ReferIdx(g);
        if(idy0 == BDDDG_NIL)
        {
          bddword ndy0 = NewNdx(g, BDDDG_OR);
          if(ndy0 == BDDDG_NIL) return BDDDG_NIL;
          idy0 = BDDDG_PackIdx(ndy0, 0);
          lkx = _nodeA[ndx0]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	      if(LinkNodes(idy0, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else if(fin0 - fin2 == 1)
      {
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    { idy0 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }
      else idy0 = BDDDG_InvSet(_c1);
  
      bddword idy1; 
      if(fin1 - fin2 > 1)
      {
        BDD g = 0;
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	  {
	    BDD g2 = _nodeA[BDDDG_Ndx(idt)]._f;
	    g |= BDDDG_Inv(idt)? ~g2: g2;
	  }
          lkx = _linkA[lkx]._nxt;
        }
        idy1 = ReferIdx(g);
        if(idy1 == BDDDG_NIL)
        {
          bddword ndy1 = NewNdx(g, BDDDG_OR);
          if(ndy1 == BDDDG_NIL) return BDDDG_NIL;
          idy1 = BDDDG_PackIdx(ndy1, 0);
          lkx = _nodeA[ndx1]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	      if(LinkNodes(idy1, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else if(fin1 - fin2 == 1)
      {
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    { idy1 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }
      else idy1 = BDDDG_InvSet(_c1);
  
      bddword ndy = NewNdx(BDDDG_Inv(idx0)? ~f: f, BDDDG_OR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx0));
      lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {
        bddword idt = _linkA[lkx]._idx;
	if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
          if(LinkNodes(idy, idt)) return BDDDG_NIL;
        lkx = _linkA[lkx]._nxt;
      }
  
      MarkSweep(idx0);
      if(Merge3(idy, idy0, idy1)) return BDDDG_NIL;;
      return idy;
    }
    MarkSweep(idx0);
  }

  // Case1-XOR ?
  if(_nodeA[ndx0]._type == BDDDG_XOR && _nodeA[ndx1]._type == BDDDG_XOR)
  {
    int fin0 = 0;
    int fin1 = 0;
    int fin2 = 0;
  
    fin0 = Mark1(idx0);
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark != 0)
        { fin2++; _nodeA[BDDDG_Ndx(idt)]._mark = 3; }
      lkx = _linkA[lkx]._nxt;
      fin1++;
    }

    if(fin2 > 0)
    {
      bddword idy0; 
      if(fin0 - fin2 > 1)
      {
        BDD g = 0;
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    g ^= _nodeA[BDDDG_Ndx(idt)]._f;
          lkx = _linkA[lkx]._nxt;
        }
        idy0 = ReferIdx(g);
        if(idy0 == BDDDG_NIL)
        {
          bddword ndy0 = NewNdx(g, BDDDG_XOR);
          if(ndy0 == BDDDG_NIL) return BDDDG_NIL;
          idy0 = BDDDG_PackIdx(ndy0, 0);
          lkx = _nodeA[ndx0]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	      if(LinkNodes(idy0, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else if(fin0 - fin2 == 1)
      {
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    { idy0 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }
      else idy0 = BDDDG_InvSet(_c1);

      bddword idy1; 
      if(fin1 - fin2 > 1)
      {
        BDD g = 0;
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    g ^= _nodeA[BDDDG_Ndx(idt)]._f;
          lkx = _linkA[lkx]._nxt;
        }
        idy1 = ReferIdx(g);
        if(idy1 == BDDDG_NIL)
        {
          bddword ndy1 = NewNdx(g, BDDDG_XOR);
          if(ndy1 == BDDDG_NIL) return BDDDG_NIL;
          idy1 = BDDDG_PackIdx(ndy1, 0);
          lkx = _nodeA[ndx1]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	      if(LinkNodes(idy1, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else if(fin1 - fin2 == 1)
      {
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark != 3)
	    { idy1 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }
      else idy1 = BDDDG_InvSet(_c1);
  
      if(BDDDG_Inv(idx0)) idy0 = BDDDG_InvAlt(idy0);
      if(BDDDG_Inv(idx1)) idy1 = BDDDG_InvAlt(idy1);
  
      bddword ndy = NewNdx(f, BDDDG_XOR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, 0);
      lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {
        bddword idt = _linkA[lkx]._idx;
	if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
          if(LinkNodes(idy, idt)) return BDDDG_NIL;
        lkx = _linkA[lkx]._nxt;
      }
  
      MarkSweep(idx0);
      if(Merge3(idy, idy0, idy1)) return BDDDG_NIL;;
      return (f == _nodeA[ndy]._f)? idy: BDDDG_InvAlt(idy);
    } 
    MarkSweep(idx0);
  }

  // Case2-OR(a)? part 0 
  if(_nodeA[ndx1]._type == BDDDG_OR)
  {
    int fin1 = 0;
    int fin2 = 0;
    _nodeA[ndx0]._mark = 1;
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark == 1)
      {
        fin2++;
	if(BDDDG_Inv(idt)) _nodeA[BDDDG_Ndx(idt)]._mark = 2;
      }
      fin1++;
      lkx = _linkA[lkx]._nxt;
    }

    if(fin2 > 0 &&
      (_nodeA[ndx0]._mark == 2) == (BDDDG_Inv(idx0)!=BDDDG_Inv(idx1)))
    {
      bddword idy1;
      if(fin1 > 2)
      {
        BDD g = 0;
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	  {
	    BDD g2 = _nodeA[BDDDG_Ndx(idt)]._f;
	    g |= BDDDG_Inv(idt)? ~g2: g2;
	  }
          lkx = _linkA[lkx]._nxt;
        }
        idy1 = ReferIdx(g);
        if(idy1 == BDDDG_NIL)
        {
          bddword ndy1 = NewNdx(g, BDDDG_OR);
          if(ndy1 == BDDDG_NIL) return BDDDG_NIL;
          idy1 = BDDDG_PackIdx(ndy1, 0);
          lkx = _nodeA[ndx1]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
            if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	      if(LinkNodes(idy1, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else
      {
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    { idy1 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }

      bddword ndy = NewNdx(BDDDG_Inv(idx1)? ~f: f, BDDDG_OR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx1));
      if(LinkNodes(idy, (_nodeA[ndx0]._mark==1)?
        BDDDG_InvReset(idx0): BDDDG_InvSet(idx0)) )
        return BDDDG_NIL;
  
      _nodeA[ndx0]._mark = 0;
      if(Merge3(idy, BDDDG_InvSet(_c1), idy1)) return BDDDG_NIL;;
      return idy;
    }
    _nodeA[ndx0]._mark = 0;
  }

  // Case2-OR(a)? part 1 
  if(_nodeA[ndx0]._type == BDDDG_OR)
  {
    int fin0 = 0;
    int fin2 = 0;
    _nodeA[ndx1]._mark = 1;
    bddword lkx = _nodeA[ndx0]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark == 1)
      {
        fin2++;
	if(BDDDG_Inv(idt)) _nodeA[BDDDG_Ndx(idt)]._mark = 2;
      }
      fin0++;
      lkx = _linkA[lkx]._nxt;
    }

    if(fin2 > 0 &&
      (_nodeA[ndx1]._mark == 2) == (BDDDG_Inv(idx0)!=BDDDG_Inv(idx1)))
    {
      bddword idy0;
      if(fin0 > 2)
      {
        BDD g = 0;
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	  {
	    BDD g2 = _nodeA[BDDDG_Ndx(idt)]._f;
	    g |= BDDDG_Inv(idt)? ~g2: g2;
	  }
          lkx = _linkA[lkx]._nxt;
        }
        idy0 = ReferIdx(g);
        if(idy0 == BDDDG_NIL)
        {
          bddword ndy0 = NewNdx(g, BDDDG_OR);
          if(ndy0 == BDDDG_NIL) return BDDDG_NIL;
          idy0 = BDDDG_PackIdx(ndy0, 0);
          lkx = _nodeA[ndx0]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
            if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	      if(LinkNodes(idy0, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else
      {
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    { idy0 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }

      bddword ndy = NewNdx(BDDDG_Inv(idx0)? ~f: f, BDDDG_OR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, BDDDG_Inv(idx0));
      if(LinkNodes(idy, (_nodeA[ndx1]._mark==1)?
        BDDDG_InvReset(idx1): BDDDG_InvSet(idx1)) )
        return BDDDG_NIL;
  
      _nodeA[ndx1]._mark = 0;
      if(Merge3(idy, idy0, BDDDG_InvSet(_c1))) return BDDDG_NIL;;
      return idy;
    }
    _nodeA[ndx1]._mark = 0;
  }

  // Case2-XOR(a)? part 0 
  if(_nodeA[ndx1]._type == BDDDG_XOR)
  {
    int fin1 = 0;
    int fin2 = 0;
    _nodeA[ndx0]._mark = 1;
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark == 1) fin2++;
      fin1++;
      lkx = _linkA[lkx]._nxt;
    }

    if(fin2 > 0)
    {
      bddword idy1;
      if(fin1 > 2)
      {
        BDD g = 0;
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    g ^= _nodeA[BDDDG_Ndx(idt)]._f;
          lkx = _linkA[lkx]._nxt;
        }
        idy1 = ReferIdx(g);
        if(idy1 == BDDDG_NIL)
        {
          bddword ndy1 = NewNdx(g, BDDDG_XOR);
          if(ndy1 == BDDDG_NIL) return BDDDG_NIL;
          idy1 = BDDDG_PackIdx(ndy1, 0);
          lkx = _nodeA[ndx1]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
            if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	      if(LinkNodes(idy1, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else
      {
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    { idy1 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }

      bddword idy0 = BDDDG_InvSet(_c1);

      if(BDDDG_Inv(idx0)) idy0 = BDDDG_InvAlt(idy0);
      if(BDDDG_Inv(idx1)) idy1 = BDDDG_InvAlt(idy1);

      bddword ndy = NewNdx(f, BDDDG_XOR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, 0);
      if(LinkNodes(idy, BDDDG_InvReset(idx0)))
        return BDDDG_NIL;
  
      _nodeA[ndx0]._mark = 0;
      if(Merge3(idy, idy0, idy1)) return BDDDG_NIL;;
      return (f == _nodeA[ndy]._f)? idy: BDDDG_InvAlt(idy);
    } 
    _nodeA[ndx0]._mark = 0;
  }

  // Case2-XOR(a)? part 1 
  if(_nodeA[ndx0]._type == BDDDG_XOR)
  {
    int fin0 = 0;
    int fin2 = 0;
    _nodeA[ndx1]._mark = 1;
    bddword lkx = _nodeA[ndx0]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark == 1) fin2++;
      fin0++;
      lkx = _linkA[lkx]._nxt;
    }

    if(fin2 > 0)
    {
      bddword idy0;
      if(fin0 > 2)
      {
        BDD g = 0;
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    g ^= _nodeA[BDDDG_Ndx(idt)]._f;
          lkx = _linkA[lkx]._nxt;
        }
        idy0 = ReferIdx(g);
        if(idy0 == BDDDG_NIL)
        {
          bddword ndy0 = NewNdx(g, BDDDG_XOR);
          if(ndy0 == BDDDG_NIL) return BDDDG_NIL;
          idy0 = BDDDG_PackIdx(ndy0, 0);
          lkx = _nodeA[ndx0]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
            if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	      if(LinkNodes(idy0, idt)) return BDDDG_NIL;
            lkx = _linkA[lkx]._nxt;
	  }
        }
      }
      else
      {
        lkx = _nodeA[ndx0]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 0)
	    { idy0 = _linkA[lkx]._idx; break; }
          lkx = _linkA[lkx]._nxt;
        }
      }

      bddword idy1 = BDDDG_InvSet(_c1);

      if(BDDDG_Inv(idx0)) idy0 = BDDDG_InvAlt(idy0);
      if(BDDDG_Inv(idx1)) idy1 = BDDDG_InvAlt(idy1);

      bddword ndy = NewNdx(f, BDDDG_XOR);
      if(ndy == BDDDG_NIL) return BDDDG_NIL;
      bddword idy = BDDDG_PackIdx(ndy, 0);
      if(LinkNodes(idy, BDDDG_InvReset(idx1)))
        return BDDDG_NIL;
  
      _nodeA[ndx1]._mark = 0;
      if(Merge3(idy, idy0, idy1)) return BDDDG_NIL;;
      return (f == _nodeA[ndy]._f)? idy: BDDDG_InvAlt(idy);
    } 
    _nodeA[ndx1]._mark = 0;
  }


  // Case1-OTHER ?
  if(_nodeA[ndx0]._type == BDDDG_OTHER
    && _nodeA[ndx1]._type == BDDDG_OTHER)
  {
    int fin0 = 0;
    int fin1 = 0;
    int fin2 = 0;
  
    fin0 = Mark1(idx0);
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idt = _linkA[lkx]._idx;
      if(_nodeA[BDDDG_Ndx(idt)]._mark != 0)
        { fin2++; _nodeA[BDDDG_Ndx(idt)]._mark = 3; }
      lkx = _linkA[lkx]._nxt;
      fin1++;
    }

    // Case1-OTHER(a) ?
    if(fin2 > 0 && fin0 - fin2 == 1 && fin1 - fin2 == 1)
    {
      bddword idy0;
      lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {

        idy0 = _linkA[lkx]._idx;
        if(_nodeA[BDDDG_Ndx(idy0)]._mark != 3) break;
        lkx = _linkA[lkx]._nxt;
      }
      bddword idy1;
      lkx = _nodeA[ndx1]._lkx;
      while(lkx != BDDDG_NIL)
      {

        idy1 = _linkA[lkx]._idx;
        if(_nodeA[BDDDG_Ndx(idy1)]._mark != 3) break;
        lkx = _linkA[lkx]._nxt;
      }

      BDD f0 = _nodeA[ndx0]._f;
      BDD f1 = _nodeA[ndx1]._f;
      BDD g0 = _nodeA[BDDDG_Ndx(idy0)]._f;
      BDD g1 = _nodeA[BDDDG_Ndx(idy1)]._f;

      if(Func0(f0, g0) == Func0(f1, g1) &&
         Func0(f0, ~g0) == Func0(f1, ~g1))
      {
        bddword ndy = NewNdx(f, BDDDG_OTHER);
        if(ndy == BDDDG_NIL) return BDDDG_NIL;
        bddword idy = BDDDG_PackIdx(ndy, 0);
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
            if(LinkNodes(idy, idt)) return BDDDG_NIL;
          lkx = _linkA[lkx]._nxt;
        }
  
        MarkSweep(idx0);
        if(Merge3(idy, idy0, idy1)) return BDDDG_NIL;;
        return idy;
      }

      if(Func0(f0, g0) == Func0(f1, ~g1) &&
         Func0(f0, ~g0) == Func0(f1, g1))
      {
        bddword ndy = NewNdx(f, BDDDG_OTHER);
        if(ndy == BDDDG_NIL) return BDDDG_NIL;
        bddword idy = BDDDG_PackIdx(ndy, 0);
        lkx = _nodeA[ndx1]._lkx;
        while(lkx != BDDDG_NIL)
        {
          bddword idt = _linkA[lkx]._idx;
          if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
            if(LinkNodes(idy, idt)) return BDDDG_NIL;
          lkx = _linkA[lkx]._nxt;
        }
  
        MarkSweep(idx0);
        if(Merge3(idy, idy0, BDDDG_InvAlt(idy1))) return BDDDG_NIL;;
        return idy;
      }

    }
    MarkSweep(idx0);

    // Case1-OTHER(b) ?
    if(fin2 > 1 && fin0 == fin2 && fin1 == fin2)
    {
      BDD f0 = f.At0(top);
      BDD f1 = f.At1(top);
      bddword idy0;
      lkx = _nodeA[ndx0]._lkx;
      while(lkx != BDDDG_NIL)
      {
        idy0 = _linkA[lkx]._idx;
	BDD g0 = _nodeA[BDDDG_Ndx(idy0)]._f;
        if(Func0(f0, g0) == Func0(f1, ~g0) &&
           Func0(f0, ~g0) == Func0(f1, g0) )
	{
          bddword ndy = NewNdx(f, BDDDG_OTHER);
          if(ndy == BDDDG_NIL) return BDDDG_NIL;
          bddword idy = BDDDG_PackIdx(ndy, 0);
          bddword lkx2 = _nodeA[ndx0]._lkx;
          while(lkx2 != BDDDG_NIL)
          {
            if(lkx != lkx2)
	      if(LinkNodes(idy, _linkA[lkx2]._idx)) return BDDDG_NIL;
            lkx2 = _linkA[lkx2]._nxt;
          }
    
	  if(Merge3(idy, idy0, BDDDG_InvAlt(idy0))) return BDDDG_NIL;
          return idy;
	}
        lkx = _linkA[lkx]._nxt;
      }
    }
  }

  // Case2-OTHER ? part 0
  if(_nodeA[ndx1]._type == BDDDG_OTHER)
  {
    Mark2R(idx0);
    bddword lkx = _nodeA[ndx1]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idy1 = _linkA[lkx]._idx;
      if(MarkChkR(idy1) == 0)
      {
	BDD f0 = _nodeA[ndx0]._f;
	f0 = BDDDG_Inv(idx0)? ~f0: f0;
	BDD f1 = _nodeA[ndx1]._f;
	f1 = BDDDG_Inv(idx1)? ~f1: f1;
	BDD g1 = _nodeA[BDDDG_Ndx(idy1)]._f;
	BDD h = Func0(f1, g1);
        //if(h == f0 || h == ~f0)
        if(h == f0)
	{
          bddword ndy = NewNdx(f, BDDDG_OTHER);
          if(ndy == BDDDG_NIL) return BDDDG_NIL;
          bddword idy = BDDDG_PackIdx(ndy, 0);
          bddword lkx2 = _nodeA[ndx1]._lkx;
          while(lkx2 != BDDDG_NIL)
          {
            if(lkx != lkx2)
	      if(LinkNodes(idy, _linkA[lkx2]._idx)) return BDDDG_NIL;
            lkx2 = _linkA[lkx2]._nxt;
          }
    
          MarkSweepR(idx0);
	  if(Merge3(idy, BDDDG_InvAlt(_c1), idy1)) return BDDDG_NIL;
          return idy;
        }
	h = Func0(f1, ~g1);
        //if(h == f0 || h == ~f0)
        if(h == f0)
	{
          bddword ndy = NewNdx(f, BDDDG_OTHER);
          if(ndy == BDDDG_NIL) return BDDDG_NIL;
          bddword idy = BDDDG_PackIdx(ndy, 0);
          bddword lkx2 = _nodeA[ndx1]._lkx;
          while(lkx2 != BDDDG_NIL)
          {
            if(lkx != lkx2)
	      if(LinkNodes(idy, _linkA[lkx2]._idx)) return BDDDG_NIL;
            lkx2 = _linkA[lkx2]._nxt;
          }
    
          MarkSweepR(idx0);
	  if(Merge3(idy, _c1, idy1)) return BDDDG_NIL;
          return idy;
        }
      }
      lkx = _linkA[lkx]._nxt;
    }
    MarkSweepR(idx0);
  }
      
  // Case2-OTHER ? part 1
  if(_nodeA[ndx0]._type == BDDDG_OTHER)
  {
    Mark2R(idx1);
    bddword lkx = _nodeA[ndx0]._lkx;
    while(lkx != BDDDG_NIL)
    {
      bddword idy0 = _linkA[lkx]._idx;
      if(MarkChkR(idy0) == 0)
      {
	BDD f0 = _nodeA[ndx0]._f;
	f0 = BDDDG_Inv(idx0)? ~f0: f0;
	BDD f1 = _nodeA[ndx1]._f;
	f1 = BDDDG_Inv(idx1)? ~f1: f1;
	BDD g0 = _nodeA[BDDDG_Ndx(idy0)]._f;
	BDD h = Func0(f0, g0);
        //if(h == f1 || f == ~f1)
        if(h == f1)
	{
          bddword ndy = NewNdx(f, BDDDG_OTHER);
          if(ndy == BDDDG_NIL) return BDDDG_NIL;
          bddword idy = BDDDG_PackIdx(ndy, 0);
          bddword lkx2 = _nodeA[ndx0]._lkx;
          while(lkx2 != BDDDG_NIL)
          {
            if(lkx != lkx2)
	      if(LinkNodes(idy, _linkA[lkx2]._idx)) return BDDDG_NIL;
            lkx2 = _linkA[lkx2]._nxt;
          }
    
          MarkSweepR(idx1);
	  if(Merge3(idy, idy0, BDDDG_InvAlt(_c1))) return BDDDG_NIL;
          return idy;
        }
	h = Func0(f0, ~g0);
        //if(h == f1 || f == ~f1)
        if(h == f1)
	{
          bddword ndy = NewNdx(f, BDDDG_OTHER);
          if(ndy == BDDDG_NIL) return BDDDG_NIL;
          bddword idy = BDDDG_PackIdx(ndy, 0);
          bddword lkx2 = _nodeA[ndx0]._lkx;
          while(lkx2 != BDDDG_NIL)
          {
            if(lkx != lkx2)
	      if(LinkNodes(idy, _linkA[lkx2]._idx)) return BDDDG_NIL;
            lkx2 = _linkA[lkx2]._nxt;
          }
    
          MarkSweepR(idx1);
	  if(Merge3(idy, idy0, _c1)) return BDDDG_NIL;
          return idy;
        }
      }
      lkx = _linkA[lkx]._nxt;
    }
    MarkSweepR(idx1);
  }
      
  // Case3-OTHER?
  MarkSweepR(idx0);
  MarkSweepR(idx1);
  Mark2R(idx0);
  Mark2R(idx1);
  Mark3R(idx0);
  Mark3R(idx1);

  bddword idx = ReferIdx(BDDvar(top));
  if(idx == BDDDG_NIL)
  {
    bddword ndx = NewNdx(BDDvar(top), BDDDG_LIT);
    if(ndx == BDDDG_NIL) return BDDDG_NIL;
    idx = BDDDG_PackIdx(ndx, 0);
  }

  bddword ndy = NewNdx(f, BDDDG_OTHER);
  if(ndy == BDDDG_NIL) return BDDDG_NIL;
  bddword idy = BDDDG_PackIdx(ndy, 0);
  if(LinkNodes(idy, idx)) return BDDDG_NIL;
  if(LinkNodesC3(idy, idx0)) return BDDDG_NIL;
  if(LinkNodesC3(idy, idx1)) return BDDDG_NIL;
  PhaseSweep(idy);
  MarkSweepR(idx0);
  MarkSweepR(idx1);
  return idy;
}

int BDDDG::LinkNodesC3(bddword idy, bddword idx)
{
  bddword ndx = BDDDG_Ndx(idx);
  bddword lkx;
  switch(_nodeA[ndx]._mark)
  {
  case 1:
    lkx = _nodeA[ndx]._lkx;
    while(lkx != BDDDG_NIL)
    {
      if(LinkNodesC3(idy, _linkA[lkx]._idx)) return 1;
      lkx = _linkA[lkx]._nxt;
    }
    break;
  case 2:
    if(LinkNodes(idy, idx)) return 1;
    _nodeA[ndx]._mark = 9;
    break;
  case 3:
    if(LinkNodes(idy, idx)) return 1;
    break;
  case 4:
    {
      lkx = _nodeA[ndx]._lkx;
      while(lkx != BDDDG_NIL)
      {
        bddword idt = _linkA[lkx]._idx;
	bddword ndxP = _nodeA[BDDDG_Ndx(idt)]._ndxP;
	if(ndxP != BDDDG_NIL && ndxP != ndx)
	{
	  if(_nodeA[ndxP]._type == BDDDG_OR &&
	     _nodeA[ndx]._type == BDDDG_OR )
	  {
	    BDD g = 0;
	    int fin = 0;
	    bddword lkx2 = lkx;
            while(lkx2 != BDDDG_NIL)
	    {
              bddword idt2 = _linkA[lkx2]._idx;
	      if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	      {
	        if(_nodeA[BDDDG_Ndx(idt2)]._invP
		   != (char) BDDDG_Inv(idt2))
	          _nodeA[BDDDG_Ndx(idt2)]._ndxP = BDDDG_NIL;
		else
		{
	          BDD g2 = _nodeA[BDDDG_Ndx(idt2)]._f;
	          g |= BDDDG_Inv(idt2)? ~g2:g2;
		  fin++;
		}
	      }
              lkx2 = _linkA[lkx2]._nxt;
	    }
	    if(fin >= 2)
	    {
              bddword idy0 = ReferIdx(g);
              if(idy0 == BDDDG_NIL)
	      {
                bddword ndy0 = NewNdx(g, BDDDG_OR);
                if(ndy0 == BDDDG_NIL) return 1;
                idy0 = BDDDG_PackIdx(ndy0, 0);
	        lkx2 = lkx;
                while(lkx2 != BDDDG_NIL)
	        {
                  bddword idt2 = _linkA[lkx2]._idx;
	          if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
		    if(LinkNodes(idy0, idt2)) return 1;
                  lkx2 = _linkA[lkx2]._nxt;
	        }
	      }
              if(LinkNodes(idy, idy0)) return 1;

	      lkx2 = lkx;
              while(lkx2 != BDDDG_NIL)
	      {
                bddword idt2 = _linkA[lkx2]._idx;
	        if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	          _nodeA[BDDDG_Ndx(idt2)]._mark = 9;
                lkx2 = _linkA[lkx2]._nxt;
	      }
	    }
	    lkx2 = lkx;
            while(lkx2 != BDDDG_NIL)
	    {
              bddword idt2 = _linkA[lkx2]._idx;
	      if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	        _nodeA[BDDDG_Ndx(idt2)]._ndxP = BDDDG_NIL;
              lkx2 = _linkA[lkx2]._nxt;
	    }
	  }

	  if(_nodeA[ndxP]._type == BDDDG_XOR &&
	     _nodeA[ndx]._type == BDDDG_XOR )
	  {
	    BDD g = 0;
	    int fin = 0;
	    bddword lkx2 = lkx;
            while(lkx2 != BDDDG_NIL)
	    {
              bddword idt2 = _linkA[lkx2]._idx;
	      if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	      {
	        g ^= _nodeA[BDDDG_Ndx(idt2)]._f;
		fin++;
	      }
              lkx2 = _linkA[lkx2]._nxt;
	    }
	    if(fin >= 2)
	    {
              bddword idy0 = ReferIdx(g);
              if(idy0 == BDDDG_NIL)
	      {
                bddword ndy0 = NewNdx(g, BDDDG_XOR);
                if(ndy0 == BDDDG_NIL) return 1;
                idy0 = BDDDG_PackIdx(ndy0, 0);
	        lkx2 = lkx;
                while(lkx2 != BDDDG_NIL)
	        {
                  bddword idt2 = _linkA[lkx2]._idx;
	          if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
		    if(LinkNodes(idy0, idt2)) return 1;
                  lkx2 = _linkA[lkx2]._nxt;
	        }
	      }
              if(LinkNodes(idy, idy0)) return 1;

	      lkx2 = lkx;
              while(lkx2 != BDDDG_NIL)
	      {
                bddword idt2 = _linkA[lkx2]._idx;
	        if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	          _nodeA[BDDDG_Ndx(idt2)]._mark = 9;
                lkx2 = _linkA[lkx2]._nxt;
	      }
	    }
	    lkx2 = lkx;
            while(lkx2 != BDDDG_NIL)
	    {
              bddword idt2 = _linkA[lkx2]._idx;
	      if(ndxP == _nodeA[BDDDG_Ndx(idt2)]._ndxP)
	        _nodeA[BDDDG_Ndx(idt2)]._ndxP = BDDDG_NIL;
              lkx2 = _linkA[lkx2]._nxt;
	    }
	  }
	}
        lkx = _linkA[lkx]._nxt;
      }


      if(_nodeA[ndx]._type == BDDDG_OR)
      {
        BDD g = 0;
	int fin = 0;
        lkx = _nodeA[ndx]._lkx;
        while(lkx != BDDDG_NIL)
	{
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	  {
	    g |= _nodeA[BDDDG_Ndx(idt)]._f;
	    fin++;
	  }
          lkx = _linkA[lkx]._nxt;
	}
	if(fin >= 2)
	{
          bddword idy0 = ReferIdx(g);
          if(idy0 == BDDDG_NIL)
	  {
            bddword ndy0 = NewNdx(g, BDDDG_OR);
            if(ndy0 == BDDDG_NIL) return 1;
            idy0 = BDDDG_PackIdx(ndy0, 0);
            lkx = _nodeA[ndx]._lkx;
            while(lkx != BDDDG_NIL)
	    {
              bddword idt = _linkA[lkx]._idx;
	      if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	        if(LinkNodes(idy0, idt)) return 1;
              lkx = _linkA[lkx]._nxt;
	    }
	  }
	  if(LinkNodes(idy, idy0)) return 1;
          lkx = _nodeA[ndx]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	      _nodeA[BDDDG_Ndx(idt)]._mark = 9;
            lkx = _linkA[lkx]._nxt;
	  }
	}
      }

      if(_nodeA[ndx]._type == BDDDG_XOR)
      {
        BDD g = 0;
	int fin = 0;
        lkx = _nodeA[ndx]._lkx;
        while(lkx != BDDDG_NIL)
	{
          bddword idt = _linkA[lkx]._idx;
	  if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	  {
	    g ^= _nodeA[BDDDG_Ndx(idt)]._f;
	    fin++;
	  }
          lkx = _linkA[lkx]._nxt;
	}
	if(fin >= 2)
	{
          bddword idy0 = ReferIdx(g);
          if(idy0 == BDDDG_NIL)
	  {
            bddword ndy0 = NewNdx(g, BDDDG_XOR);
            if(ndy0 == BDDDG_NIL) return 1;
            idy0 = BDDDG_PackIdx(ndy0, 0);
            lkx = _nodeA[ndx]._lkx;
            while(lkx != BDDDG_NIL)
	    {
              bddword idt = _linkA[lkx]._idx;
	      if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	        if(LinkNodes(idy0, idt)) return 1;
              lkx = _linkA[lkx]._nxt;
	    }
	  }
	  if(LinkNodes(idy, idy0)) return 1;
          lkx = _nodeA[ndx]._lkx;
          while(lkx != BDDDG_NIL)
	  {
            bddword idt = _linkA[lkx]._idx;
	    if(_nodeA[BDDDG_Ndx(idt)]._mark == 3)
	      _nodeA[BDDDG_Ndx(idt)]._mark = 9;
            lkx = _linkA[lkx]._nxt;
	  }
	}
      }
    }

    lkx = _nodeA[ndx]._lkx;
    while(lkx != BDDDG_NIL)
    {
      if(LinkNodesC3(idy, _linkA[lkx]._idx)) return 1;
      lkx = _linkA[lkx]._nxt;
    }
    break;
  case 9:
    break;
  default:
    cerr << "<ERROR> BDDDG::LinkNodesC3(): wrong case (";
    cerr << _nodeA[ndx]._mark << ")\n";
    exit(1);
  }
  return 0;
}

BDD BDDDG::Func0(BDD f, BDD g)
{
  BDD h = f;
  while(g != 0)
  {
    int top = g.Top();
    BDD g0 = g.At0(top);
    if(g0 != 1)
    {
      g = g0;
      h = h.At0(top);
    }
    else
    {
      g = g.At1(top);
      h = h.At1(top);
    }
  }
  return h;
}

int BDDDG::Merge3(bddword idy, bddword idy0, bddword idy1)
{
  int top = _nodeA[BDDDG_Ndx(idy)]._f.Top();
  BDD h0 = BDDDG_Inv(idy0)?
    ~(_nodeA[BDDDG_Ndx(idy0)]._f): _nodeA[BDDDG_Ndx(idy0)]._f;
  BDD h1 = BDDDG_Inv(idy1)?
    ~(_nodeA[BDDDG_Ndx(idy1)]._f): _nodeA[BDDDG_Ndx(idy1)]._f;
  BDD h = (~BDDvar(top) & h0) | (BDDvar(top) & h1);
  bddword idx = ReferIdx(h);
  if(idx == BDDDG_NIL) idx = Merge(h, idy0, idy1);
  if(idx == BDDDG_NIL) return 1;
  if(LinkNodes(idy, idx)) return BDDDG_NIL;
  PhaseSweep(idy);
  return 0;
}

int BDDDG::PrintDecomp(BDD f)
{
  bddword idx = Decomp(f);
  if(idx == BDDDG_NIL) return 1;
  Print0(idx);
  cout << "\n";
  return 0;
}

void BDDDG::Print0(bddword idx)
{
  if(BDDDG_Inv(idx)) cout << "!";
  bddword ndx = BDDDG_Ndx(idx);
  bddword lkx;
  switch(_nodeA[ndx]._type)
  {
  case BDDDG_C1:
    cout << "1 ";
    break;
  case BDDDG_LIT:
    cout << "x" << _nodeA[ndx]._f.Top() << " ";
    break;
  case BDDDG_OR:
    cout << "OR( ";
    lkx = _nodeA[ndx]._lkx;
    while(lkx != BDDDG_NIL)
    {
      Print0(_linkA[lkx]._idx);
      lkx = _linkA[lkx]._nxt;
    }
    cout << ") ";
    break;
  case BDDDG_XOR:
    cout << "XOR( ";
    lkx = _nodeA[ndx]._lkx;
    while(lkx != BDDDG_NIL)
    {
      Print0(_linkA[lkx]._idx);
      lkx = _linkA[lkx]._nxt;
    }
    cout << ") ";
    break;
  case BDDDG_OTHER:
    cout << "[ ";
    lkx = _nodeA[ndx]._lkx;
    while(lkx != BDDDG_NIL)
    {
      Print0(_linkA[lkx]._idx);
      lkx = _linkA[lkx]._nxt;
    }
    cout << "] ";
    break;
  default:
    cerr << "<ERROR> BDDDG::Print0: wrong case (";
    cerr << (int)_nodeA[ndx]._type << ")\n";
    exit(1);
  }
}

