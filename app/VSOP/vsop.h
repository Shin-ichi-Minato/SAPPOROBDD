// VSOP - Header (v1.39)
// Shin-ichi MINATO (Nov. 22, 2013)

#define PROMPT "vsop> "
#define DOCUMENT "vsop.help"

int yyparse();

struct VarEntry;

class VarTable
{
  int _used;
  int _hashsize;
  VarEntry* _wheel;
  VarEntry** _index;

  void Enlarge(void);
public:
  VarEntry* GetEntry(char *);
  VarTable(int size = 64);
  ~VarTable(void);
  int GetID(char *);
  char* GetName(int);
  int GetValue(int);
  int GetGID(int);
  void SetB(char *, int);
  void SetB(char *, int, int);
  void SetT(char *, int);
  void SetT0(int, char *);
  void SetT(char *, int, int);
  int Used(void);
};

struct FuncEntry;
class CtoI;

class FuncTable
{
  int _used;
  int _hashsize;
  FuncEntry* _wheel;

  void Enlarge(void);
public:
  FuncEntry* GetEntry(char *);
  FuncTable(int size = 256);
  ~FuncTable(void);
  int CheckNew(char *);
  CtoI& GetCtoI(char *);
  void Set(char *, CtoI &);
  int Used(void);
  char GetAutoExportID(char *);
  void SetAutoExportID(char *, char);
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

extern int PrintCtoI(CtoI);
extern int PrintCtoI_16(CtoI);
extern int PrintDigital(CtoI);
extern int PrintCase(CtoI);
extern int MapAll(CtoI);
extern int MapSel(CtoI);

extern int PrintDecomp(CtoI);
extern int PrintDecompDot(CtoI);
