// Perm - Header (v0.4)
// Shin-ichi MINATO (May 14, 2021)

#define PROMPT "perm> "
#define DOCUMENT "perm.help"
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
  int Used(void);
};

struct FuncEntry;
class PiDD;

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
  PiDD& GetPiDD(char *);
  void Set(char *, PiDD);
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

extern void PermEnum(PiDD);
extern void PermEnum2(PiDD);
