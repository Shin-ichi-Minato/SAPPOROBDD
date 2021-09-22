// BEM-II - Header (SAPPORO-1.31)
// Shin-ichi MINATO (May 14, 2021)

#define PROMPT "> "
#define DOCUMENT "bemII.help"
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
  void SetB(char *, bddword);
  void SetT(char *, bddword);
  void SetT0(int, char *);
  int Used(void);
};

struct FuncEntry;
class BtoI;

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
  BtoI& GetBtoI(char *);
  void Set(char *, BtoI);
  int Used(void);
};

class BOut
{
  short _column;
public:
  BOut(void);
  BOut& operator <<(const char *);
  void Delimit(void);
  void Return(void);
};

extern VarTable VTable;
extern FuncTable FTable;
extern BOut bout;

extern void yyerror(const char *);

extern int PutList(BtoI, int xor_ = 0, int base = 10);
extern int MapAll(BtoI, int base = 10);
extern int MapSel(BtoI, int base = 10);
extern int PutASOP(BtoI);
extern int PutCase(BtoI, int base = 10);
extern int Plot(BtoI, int);
extern int PrintDecomp(BtoI);
extern int PutV(BtoI);

