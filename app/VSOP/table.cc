// VSOP Tables (v1.36)
// Shin-ichi MINATO (Dec. 18, 2010)

#include <cstring>
#include <iostream>
#include "CtoI.h"
#include "vsop.h"
using namespace std;

struct VarEntry
{
  int _val;
  int _gvar;
  char* _name;
  int _len;
  int _var;

  VarEntry(void);
  ~VarEntry(void);
};

VarEntry::VarEntry(void)
{
  _val = 0;
  _gvar = 0;
  _var = 0;
  _len = 0;
  _name = 0;
}

VarEntry::~VarEntry(void) { if(_len > 0) delete[] _name; }

typedef VarEntry* VarEntPtr;

VarTable::VarTable(int size)
{
  if(size <= 0)
  {
    cerr << "Error at VarTable. (a)\n";
    exit(1);
  }
  for(_hashsize=128; _hashsize<size; _hashsize<<=1)
    ; // empty
  _wheel = new VarEntry[_hashsize];
  _index = new VarEntPtr[_hashsize>>1];
  if(_wheel == 0 || _index == 0)
  {
    cerr << "Error at VarTable. (b)\n";
    exit(1);
  }
  for(int i=0; i<(_hashsize>>1); i++) _index[i] = 0;
  _used = 0;
}

VarTable::~VarTable()
{
  delete[] _wheel;
  delete[] _index;
}

VarEntry* VarTable::GetEntry(char* name)
{
  if(name == 0)
  {
    cerr << "Error at VarTable. (c)\n";
    exit(1);
  }
  int hash = 0;
  for(int i=0; i<32; i++)
  {
    if(name[i] == 0) break;
    hash = (hash * 123456 + name[i]) & (_hashsize - 1);
  }
  int i = hash;
  while(_wheel[i]._len > 0)
  {
    if(strcmp(name, _wheel[i]._name) == 0)
      return & _wheel[i];
    i++;
    i &= (_hashsize -1);
  }
  i = hash;
  while(_wheel[i]._len > 0)
  {
    if(_wheel[i]._var == 0) break;
    i++;
    i &= (_hashsize -1);
  }
  _wheel[i]._len = strlen(name)+1;
  _wheel[i]._name = new char[_wheel[i]._len];
  strcpy(_wheel[i]._name, name);
  return & _wheel[i];
}

int VarTable::GetID(char* name) { return GetEntry(name) -> _var; }

char* VarTable::GetName(int var)
{ return _index[var-BDDV_SysVarTop-1] -> _name; }

int VarTable::GetValue(int var)
{ return _index[var-BDDV_SysVarTop-1] -> _val; }

int VarTable::GetGID(int var)
{ return _index[var-BDDV_SysVarTop-1] -> _gvar; }

void VarTable::SetB(char* name, int val)
{
  VarEntry* entry =  GetEntry(name);
  entry -> _val = val;
  if(entry -> _var == 0)
  {
    entry -> _var = BDD_NewVarOfLev(1);
    entry -> _gvar = entry -> _var;
    _index[_used] = entry;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

void VarTable::SetB(char* name, int val, int gvar)
{
  VarEntry* entry =  GetEntry(name);
  entry -> _val = val;
  entry -> _gvar = gvar;
  if(entry -> _var == 0)
  {
    entry -> _var = BDD_NewVarOfLev(1);
    _index[_used] = entry;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

void VarTable::SetT(char* name, int val)
{
  VarEntry* entry =  GetEntry(name);
  entry -> _val = val;
  if(entry -> _var == 0)
  {
    entry -> _var = BDD_NewVar();
    entry -> _gvar = entry -> _var;
    _index[_used] = entry;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

void VarTable::SetT0(int var, char* name)
{
  VarEntry* entry =  GetEntry(name);
  entry -> _val = 1 << 15;
  if(entry -> _var == 0)
  {
    entry -> _var = var;
    entry -> _gvar = entry -> _var;
    _index[_used] = entry;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

void VarTable::SetT(char* name, int val, int gvar)
{
  VarEntry* entry =  GetEntry(name);
  entry -> _val = val;
  entry -> _gvar = gvar;
  if(entry -> _var == 0)
  {
    entry -> _var = BDD_NewVar();
    _index[_used] = entry;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

int VarTable::Used(void) { return _used; }

void VarTable::Enlarge(void)
{
  int oldsize = _hashsize;
  VarEntry* oldwheel = _wheel;
  VarEntry** oldindex = _index;

  _hashsize <<= 2;
  _wheel = new VarEntry[_hashsize];
  _index = new VarEntPtr[_hashsize>>1];
  if(_wheel == 0 || _index == 0)
  {
    cerr << "Error at VarTable. (d)\n";
    exit(1);
  }
  for(int i=0; i<(_hashsize>>1); i++) _index[i] = 0;
  _used = 0;
  for(int i=0; i<(oldsize>>1); i++)
  {
    VarEntry* oldentry = oldindex[i];
    if(oldentry)
    {
      _used++;
      VarEntry* entry = GetEntry(oldentry->_name);
      entry -> _val = oldentry -> _val;
      entry -> _gvar = oldentry -> _gvar;
      entry -> _var = oldentry -> _var;
      _index[i] = entry;
    }
  }
  delete[] oldwheel;
  delete[] oldindex;
}

//
// FuncTable class definition
//

struct FuncEntry
{
  CtoI _ctoi;
  char* _name;
  short _len;
  char _live;
  char _autoexport;

  FuncEntry(void);
  ~FuncEntry(void);
};

FuncEntry::FuncEntry(void)
{
  _ctoi = CtoI_Null();
  _name = 0;
  _len = 0;
  _live = 0;
  _autoexport = -1;
}

FuncEntry::~FuncEntry(void) { if(_len > 0) delete[] _name; }

FuncTable::FuncTable(int size)
{
  if(size <= 0)
  {
    cerr << "Error at FuncTable. (a)\n";
    exit(1);
  }
  for(_hashsize=256; _hashsize<size; _hashsize<<=1)
    ; // empty
  _wheel = new FuncEntry[_hashsize];
  if(_wheel == 0)
  {
    cerr << "Error at FuncTable. (b)\n";
    exit(1);
  }
  _used = 0;
}

FuncTable::~FuncTable() { delete[] _wheel; }

FuncEntry* FuncTable::GetEntry(char* name)
{
  if(name == 0)
  {
    cerr << "Error at FuncTable. (b)\n";
		exit(1);
  }
  int hash = 0;
  for(int i=0; i<32; i++)
  {	
    if(name[i] == 0) break;
    hash = (hash * 123456 + name[i]) & (_hashsize - 1);
  }
  int i = hash;
  while(_wheel[i]._len > 0)
  {
    if(strcmp(name, _wheel[i]._name) == 0)
      return & _wheel[i];
    i++;
    i &= (_hashsize -1);
  }
  i = hash;
  while(_wheel[i]._len > 0)
  {
    if(_wheel[i]._live == 0) break;
    i++;
    i &= (_hashsize -1);
  }
  _wheel[i]._len = strlen(name)+1;
  _wheel[i]._name = new char[_wheel[i]._len];
  strcpy(_wheel[i]._name, name);
  return & _wheel[i];
}

CtoI& FuncTable::GetCtoI(char* name) { return GetEntry(name) -> _ctoi; }
char FuncTable::GetAutoExportID(char* name) { return GetEntry(name) -> _autoexport; }

void FuncTable::SetAutoExportID(char *name, char id)
{
    FuncEntry* entry =  GetEntry(name);
    if (entry)
	entry -> _autoexport = id;
}

void FuncTable::Set(char* name, CtoI& ctoi)
{
  FuncEntry* entry =  GetEntry(name);
  entry -> _ctoi = ctoi;
  if(entry -> _live == 0)
  {
    entry -> _live = 1;
    if(++_used >= (_hashsize>>1)) Enlarge();
  }
}

int FuncTable::Used(void) { return _used; }

void FuncTable::Enlarge(void)
{
  int oldsize = _hashsize;
  FuncEntry* oldwheel = _wheel;

  _hashsize <<= 2;
  _wheel = new FuncEntry[_hashsize];
  if(_wheel == 0)
  {
    cerr << "Error at FuncTable. (b)\n";
    exit(1);
  }
  _used = 0;
  for(int i=0; i<oldsize; i++)
  if(oldwheel[i]._live != 0)
    Set(oldwheel[i]._name, oldwheel[i]._ctoi);
  delete[] oldwheel;
}

VarTable VTable;
FuncTable FTable;
