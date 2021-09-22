// BN - Header (ver 0.01)
// Shin-ichi MINATO (Jun. 19, 2006)

const int BNMaxFin = 32;

class BNHead;

class BNNode
{
  int _vhashsize;
  int* _vwheel;
  char** _vindex;
  int* _nonzero;
  int _phashsize;
  int* _pwheel;
  char** _pindex;
  int _fin[BNMaxFin];
  int* _cpt;

  int GetValWheelIndex(char *);
  int GetPrbWheelIndex(char *);
  void EnlargePrb(void);

public:
  BNHead* head;
  char* name;
  int n_val;
  int n_fin;
  int n_prb;
  int n_cpt;
  int n_fout;
  int myoffset;
  int offset[BNMaxFin];

  BNNode(void);
  ~BNNode(void);
  void SetName(char *);
  void SetNVal(int);

  char* GetVal(int);
  int GetValIndex(char *);
  void SetVal(int, char *);
  int ChkNZ(int);
  void SetNZ(int);
  
  char* GetPrb(int);
  int GetPrbIndex(char *);
  void EnterPrb(char *);
  
  int GetFin(int);
  void EnterFin(int);

  void AllocCPT(void);
  void SetCPT(int, int);
  int GetCPT(int);
};

class BNHead
{
  int _hashsize;
  int* _wheel;
  BNNode** _index;

  void Enlarge(void);
  int GetWheelIndex(char *);

public:
  char* name;
  int n_node;

  BNHead(int size = 256);
  ~BNHead(void);
  void SetName(char *);
  BNNode* GetNode(int);
  int GetIndex(char *);
  void NewNode(char *);
};

int bnparse();

extern void yyerror(char *);

