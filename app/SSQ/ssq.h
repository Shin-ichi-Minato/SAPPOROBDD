// SSQ - Header (v0.1)
// Shin-ichi MINATO (May 17, 2015)

#define PROMPT "ssq> "
#define DOCUMENT "ssq.help"
#define NDEF 400000

int yyparse();

struct VarEntry;

class VarTable
{
  int _used;
  int _hashsize;
  VarEntry* _wheel;
  VarEntry** _index;

  void Enlarge(void);
  VarEntry* GetEntry(char *);
public:
  VarTable(int size = 64);
  ~VarTable(void);
  int GetVarID(char *);
  char* GetName(int);
  bddword GetCost(int);
  void SetT(char *, bddword);
  void SetB(char *, bddword);
  int Used(void);
};

struct FuncEntry;
class SeqBDD;

class FuncTable
{
  int _used;
  int _hashsize;
  FuncEntry* _wheel;

  void Enlarge(void);
  FuncEntry* GetEntry(char *);
public:
  FuncTable(int size = 256);
  ~FuncTable(void);
  int CheckNew(char *);
  SeqBDD& GetSeqBDD(char *);
  void Set(char *, SeqBDD);
  int Used(void);
};

class BOut
{
  short _column;
public:
  BOut(void);
  BOut& operator <<(const char *);
  void Delimit(void);
  void Delimit0(void);
  void Return(void);
};

extern VarTable VTable;
extern FuncTable FTable;
extern BOut bout;

extern void yyerror(const char *);

extern void ssqprint(SeqBDD);
