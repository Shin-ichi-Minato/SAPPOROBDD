/************************************************
 * Graph Base class (SAPPORO-1.87) - Header     *
 * (C) Shin-ichi MINATO  (May 14, 2021)         *
 ************************************************/

class GBase;

#ifndef _GBase_
#define _GBase_

#include "ZBDD.h"

//typedef unsigned char GB_v; // up to 254 vertices.
typedef unsigned short GB_v; // more than 255 vertices.
typedef unsigned short GB_e;
extern const char GB_fix0;
extern const char GB_fix1;

class GBase
{
public:
  struct CacheEntry // for SimPaths
  {
    ZBDD _f; // for ZDD-constrained enumeration
    ZBDD _h;
    CacheEntry(void)
    {
      _f = -1; // for ZDD-constrained enumeration
      _h = -1; 
    }
    ~CacheEntry(void) { }
  };

  struct Vertex
  {
    int _tmp;
    GB_e _deg; // for SimPaths
    Vertex(void) { _tmp = 0; }
    ~Vertex(void) { }
  };

  struct Edge
  {
    GB_v _ev[2];
    int _tmp;
    int _cost; // for Cost-Bounded Enumeration

    char _io[2]; // for SimPaths
    char _preset; // for SimPaths
    GB_v _mtwid; //for SimPaths
    bddword _casize; // for SimPaths
    bddword _caent; // for SimPaths

    GB_v* _map; // for SimPaths
    GB_v* _cfg; // for SimPaths
    CacheEntry* _ca; //for SimPaths
    GB_v* _ca_mate; //for SimPaths
    
    ZBDD _f; // for ZDD-constrained enumeration

    Edge(void);
    ~Edge(void);
  };

  GB_v _n;
  GB_e _m;
  Vertex* _v;
  Edge* _e;

  GB_e _lastin; //for HamPaths

  int _hamilton;
  ZBDD _f; // for ZDD-constrained enumeration

  GBase(void);
  ~GBase(void);
  int Init(const int, const int);
  int Pack(void);
  int Import(FILE *);
  int SetGrid(const int, const int);
  void Print(void) const;

  ZBDD SimPaths(const GB_v, const GB_v);
  ZBDD SimCycles(void);
  int BDDvarOfEdge(const GB_e) const;
  GB_e EdgeOfBDDvar(const int) const;

  void FixEdge(const GB_e, const char);
  void SetHamilton(const int);
  void SetCond(ZBDD); // for ZDD-constrained enumeration
};

#endif // _GBase_

