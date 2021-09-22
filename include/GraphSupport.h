/************************************************
 * GraphSupport class (SAPPORO-1.83) - Header   *
 * (C) Shin-ichi MINATO  (Mar. 25, 2017)        *
 ************************************************/

class GraphSupport;

#ifndef _GraphSupport_
#define _GraphSupport_

#include "ZBDD.h"

//typedef unsigned char GS_v; // up to 254 vertices.
typedef unsigned short GS_v; // more than 255 vertices.
typedef unsigned short GS_e;
extern const char GS_fix0;
extern const char GS_fix1;

class GraphSupport
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
    GS_e _deg; // for SimPaths
    Vertex(void) { _tmp = 0; }
    ~Vertex(void) { }
  };

  struct Edge
  {
    GS_v _ev[2];
    int _tmp;

    char _io[2]; // for SimPaths
    char _preset; // for SimPaths
    GS_v _mtwid; //for SimPaths
    bddword _casize; // for SimPaths
    bddword _caent; // for SimPaths

    GS_v* _map; // for SimPaths
    GS_v* _cfg; // for SimPaths
    CacheEntry* _ca; //for SimPaths
    GS_v* _ca_mate; //for SimPaths
    
    ZBDD _f; // for ZDD-constrained enumeration

    Edge(void);
    ~Edge(void);
  };

  GS_v _n;
  GS_e _m;
  Vertex* _v;
  Edge* _e;

  GS_e _lastin; //for HamPaths

  int _hamilton;
  ZBDD _f; // for ZDD-constrained enumeration

  GraphSupport(void);
  ~GraphSupport(void);
  int Init(const int, const int);
  int Pack(void);
  int Import(FILE *);
  int SetGrid(const int, const int);
  void Print(void) const;

  ZBDD SimPaths(const GS_v, const GS_v);
  ZBDD SimCycles(void);
  int BDDvarOfEdge(const GS_e) const;
  GS_e EdgeOfBDDvar(const int) const;

  void FixEdge(const GS_e, const char);
  void SetHamilton(const int);
  void SetCond(ZBDD); // for ZDD-constrained enumeration
};

#endif // _GraphSupport_

