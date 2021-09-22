/*****************************************
 * BDDDG - Decomposition Graph           *
 * (SAPPORO-1.01) - Header               *
 * (C) Shin-ichi MINATO  (July 4, 2005)  *
 *****************************************/

#ifndef _BDDDG_
#define _BDDDG_

#include "BDD.h"

#define BDDDG_InitSize 4

#define BDDDG_NIL   BDD_MaxNode

#define BDDDG_C1    1
#define BDDDG_LIT   2
#define BDDDG_OR    3
#define BDDDG_XOR   4
#define BDDDG_OTHER 5

#define BDDDG_PackIdx(ndx,inv) (((ndx)==BDDDG_NIL)? BDDDG_NIL:(((ndx)<<1)|inv))
#define BDDDG_Ndx(idx)         (((idx)==BDDDG_NIL)? BDDDG_NIL:((idx)>>1))
#define BDDDG_Inv(idx)         ((idx)&1)
#define BDDDG_InvSet(idx)      ((idx)|1)
#define BDDDG_InvReset(idx)    ((idx)&(~1))
#define BDDDG_InvAlt(idx)      ((idx)^1)

class BDDDG
{
  struct Node;
  struct NodeLink
  {
    bddword _idx;
    bddword _nxt;
    NodeLink(void){ _idx = BDDDG_NIL; _nxt = BDDDG_NIL; }
  };

  bddword _nodeSize;
  bddword _nodeUsed;
  bddword _linkSize;
  bddword _linkUsed;
  bddword* _hashWheel;
  Node* _nodeA;
  NodeLink* _linkA;
  bddword _c1;

  bddword HashIndex(BDD);
  bddword NewNdx(BDD, char);
  int EnlargeNode(void);
  bddword NewLkx(bddword);
  int EnlargeLink(void);
  int LinkNodes(bddword, bddword);
  int Merge3(bddword, bddword, bddword);
  BDD Func0(BDD, BDD);
  int LinkNodesC3(bddword, bddword);
  void Print0(bddword);
  bddword Merge(BDD, bddword, bddword);
  bddword ReferIdx(BDD);
  void PhaseSweep(bddword);
  void MarkSweep(bddword);
  void MarkSweepR(bddword);
  int Mark1(bddword);
  void Mark2R(bddword);
  int MarkChkR(bddword);
  void Mark3R(bddword);

  struct Node
  {
    bddword _lkx;
    BDD _f;
    bddword _ndxP;
    char _invP;
    char _type;  // NULL, C1, LIT, OR, XOR, OTHER
    char _mark;
    Node(void);
    Node(BDD, char);
  };

public:
  BDDDG(void);
  ~BDDDG(void);
  void Clear(void);
  bddword NodeUsed(void);
  int PrintDecomp(BDD);
  bddword Decomp(BDD);
  bddword NodeIdx(BDD);

  friend class BDDDG_Tag;
};

class BDDDG_Tag
{
  BDDDG* _dg;
  bddword _ndx;
  bddword _lkx;
public:
  BDDDG_Tag(void);
  int Set(BDDDG *, bddword);
  bddword TopIdx(void);
  bddword NextIdx(void);
  char Type(void);
  BDD Func(void);
};

#endif // _BDDDG_

