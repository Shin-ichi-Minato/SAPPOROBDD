/****************************************
 * BDD+ Manipulator (SAPPORO-1.59)      *
 * (Hash table methods)                 *
 * (C) Shin-ichi MINATO (Dec. 10, 2013) *
 ****************************************/

#include "BDD.h"

using std::cerr;

BDD_Hash::BDD_Hash()
{
  _hashSize = 16;
  _wheel = new BDD_Entry[_hashSize];
  _amount = 0;
}

BDD_Hash::~BDD_Hash() { delete[] _wheel; }

void BDD_Hash::Clear()
{
  if(_hashSize != 0) delete[] _wheel;
  _hashSize = 16;
  _wheel = new BDD_Entry[_hashSize];
  _amount = 0;
}

BDD_Hash::BDD_Entry* BDD_Hash::GetEntry(BDD key)
{
  bddword id = key.GetID();
  bddword hash = (id+(id>>10)+(id>>20)) & (_hashSize - 1);
  bddword i = hash;
  while(_wheel[i]._key != -1)
  {
    if(key == _wheel[i]._key) return & _wheel[i];
    i++;
    i &= (_hashSize -1);
  }
  i = hash;
  while(_wheel[i]._key != -1)
  {
    if(_wheel[i]._ptr == 0) break;
    i++;
    i &= (_hashSize -1);
  }
  return & _wheel[i];
}

void BDD_Hash::Enlarge()
{
  bddword oldSize = _hashSize;
  BDD_Entry* oldWheel = _wheel;

  _hashSize <<= 2;
  _wheel = new BDD_Entry[_hashSize];
  if(_wheel == 0)
  {
    cerr << "<ERROR> BDD_Hash::Enlarge(): Memory overflow (";
    cerr << _hashSize << ")\n";
    exit(1);
  }
  _amount = 0;
  for(bddword i=0; i<oldSize; i++)
    if(oldWheel[i]._ptr != 0)
      if(oldWheel[i]._key != -1)
      	Enter(oldWheel[i]._key, oldWheel[i]._ptr);
  delete[] oldWheel;
}

void BDD_Hash::Enter(BDD key, void* ptr)
// ptr = 0 means deleting.
{
  BDD_Entry* ent = GetEntry(key);
  if(ent -> _key == -1) _amount++;
  else if(ent -> _ptr == 0) _amount++;
  if(ptr == 0) _amount--;
  ent -> _key = key;
  ent -> _ptr = ptr;
  if(_amount >= (_hashSize>>1)) Enlarge();
}

void* BDD_Hash::Refer(BDD key)
// returns 0 if not found.
{
  BDD_Entry* ent = GetEntry(key);
  if(ent -> _key == -1) return 0;
  return ent -> _ptr;
}

bddword BDD_Hash::Amount() { return _amount; }

