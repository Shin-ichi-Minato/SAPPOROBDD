/*****************************************
 * ZZBDDDG - Decomposition Graph         *
 * (SAPPORO-1.02) - Header               *
 * (C) Shin-ichi MINATO  (Aug. 8, 2005)  *
 *****************************************/

#ifndef _ZBDDDG_
#define _ZBDDDG_

#include "ZBDD.h"

#define ZBDDDG_InitSize 4

#define ZBDDDG_NIL   BDD_MaxNode

#define ZBDDDG_C0    1
#define ZBDDDG_P1    2
#define ZBDDDG_LIT   3
#define ZBDDDG_AND   4
#define ZBDDDG_OR    5
#define ZBDDDG_OTHER 6

class ZBDDDG
{
  struct Node;
  struct NodeLink
  {
    bddword _ndx;
    bddword _nxt;
    NodeLink(void){ _ndx = ZBDDDG_NIL; _nxt = ZBDDDG_NIL; }
  };

  bddword _nodeSize;
  bddword _nodeUsed;
  bddword _linkSize;
  bddword _linkUsed;
  bddword* _hashWheel;
  Node* _nodeA;
  NodeLink* _linkA;
  bddword _c0;
  bddword _c1;

  bddword HashIndex(ZBDD);
  bddword NewNdx(ZBDD, char);
  int EnlargeNode(void);
  bddword NewLkx(bddword);
  int EnlargeLink(void);
  int LinkNodes(bddword, bddword);
  int Merge3(bddword, bddword, bddword);
  ZBDD Func0(ZBDD, ZBDD);
  ZBDD Func1(ZBDD, ZBDD);
  int LinkNodesC3(bddword, bddword);
  void Print0(bddword);
  bddword Merge(ZBDD, bddword, bddword);
  bddword ReferNdx(ZBDD);
  int PhaseSweep(bddword);
  void MarkSweep(bddword);
  void MarkSweepR(bddword);
  int Mark1(bddword);
  void Mark2R(bddword);
  int MarkChkR(bddword);
  void Mark3R(bddword);
  bddword Mark4R(bddword, bddword, bddword);
  bddword Mark5R(bddword, bddword, bddword);
  void Mark6R(bddword, bddword);
  bddword AppendR(bddword, int, bddword, bddword);

  struct Node
  {
    bddword _lkx;
    ZBDD _f;
    bddword _ndxP;
    char _type;  // NULL, C1, P1, LIT, OR, XOR, OTHER
    char _mark;
    Node(void);
    Node(ZBDD, char);
  };

public:
  ZBDDDG(void);
  ~ZBDDDG(void);
  void Clear(void);
  bddword NodeUsed(void);
  int PrintDecomp(ZBDD);
  bddword Decomp(ZBDD);

  friend class ZBDDDG_Tag;
};

class ZBDDDG_Tag
{
  ZBDDDG* _dg;
  bddword _ndx;
  bddword _lkx;
public:
  ZBDDDG_Tag(void);
  int Set(ZBDDDG *, bddword);
  bddword TopNdx(void);
  bddword NextNdx(void);
  char Type(void);
  ZBDD Func(void);
};

#endif // _ZBDDDG_

