// BN Tables (ver 0.3)
// Shin-ichi MINATO (Dec. 10, 2013)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "bn.h"
using namespace std;

//
// BNHead class definition
//

BNHead::BNHead(int size)
{
  if(size <= 0)
  {
    cerr << "Error at BNHead. (1)\n";
    exit(1);
  }
  for(_hashsize=256; _hashsize<size; _hashsize<<=1)
    ; // empty
  _wheel = new int[_hashsize];
  _index = new BNNode* [_hashsize];
  if(_wheel == 0 || _index == 0)
  {
    cerr << "Error at BNHead. (2)\n";
    exit(1);
  }
  for(int i=0; i<_hashsize; i++)
  {
    _wheel[i] = -1;
    _index[i] = 0;
  }
  n_node = 0;
}

BNHead::~BNHead()
{
  for(int i=0; i<n_node; i++) delete _index[i];
  delete[] _index;
  delete[] _wheel;
  delete[] name;
}

void BNHead::SetName(char* bnname)
{
  name = new char[strlen(bnname)+1];
  if(name == 0)
  {
    cerr << "Error at BNHead. (6)\n";
    exit(1);
  }
  strcpy(name, bnname);
}

BNNode* BNHead::GetNode(int index)
{
  if(index < 0 || index >= n_node)
  {
    cerr << "Error at BNHead. (5)\n";
    exit(1);
  }
  return _index[index];
}

int BNHead::GetWheelIndex(char* nodename)
{
  if(nodename == 0)
  {
    cerr << "Error at BNHead. (3)\n";
    exit(1);
  }
  int hash = 0;
  for(int i=0; i<32; i++)
  {	
    if(nodename[i] == 0) break;
    hash = (hash * 123456 + nodename[i]) & (_hashsize - 1);
  }
  int i = hash;
  while(_wheel[i] != -1)
  {
    if(strcmp(nodename, _index[_wheel[i]]->name) == 0)
      return i;
    i++;
    i &= (_hashsize -1);
  }
  return i;
}

int BNHead::GetIndex(char* nodename)
{
  return _wheel[ GetWheelIndex(nodename) ];
}

void BNHead::NewNode(char* nodename)
{
  int wix = GetWheelIndex(nodename);
  if(_wheel[wix] != -1) return;

  BNNode* node = new BNNode();
  if(node == 0)
  {
    cerr << "Error at BNHead. (4)\n";
    exit(1);
  }
  node->head = this;
  node->SetName(nodename);
  _index[n_node] = node;
  _wheel[wix] = n_node;
  if(++n_node >= (_hashsize>>1)) Enlarge();
}

void BNHead::Enlarge(void)
{
  int oldsize = _hashsize;
  int* oldwheel = _wheel;
  BNNode** oldindex = _index;

  _hashsize <<= 2;
  _wheel = new int[_hashsize];
  _index = new BNNode* [_hashsize];
  if(_wheel == 0 || _index == 0)
  {
    cerr << "Error at FuncTable. (5)\n";
    exit(1);
  }
  for(int i=0; i<_hashsize; i++)
  {
    _wheel[i] = -1;
    _index[i] = 0;
  }
  for(int i=0; i<n_node; i++)
  {
    _index[i] = oldindex[i];
    _wheel[ GetWheelIndex(_index[i]->name) ] = i;
  }
  delete[] oldindex;
  delete[] oldwheel;
}
 
BNNode::BNNode()
{
  head = 0;
  name = 0;
  n_val = 0;
  n_fin = 0;
  n_cpt = 0;
  n_fout = 0;

  _vhashsize = 0;
  _vwheel = 0;
  _vindex = 0;
  _nonzero = 0;
  _phashsize = 0;
  _cpt = 0;
  for(int i=0; i<BNMaxFin; i++) _fin[i] = 0;

  _phashsize=16;
  n_prb = 0;
  _pwheel = new int[_phashsize];
  _pindex = new char* [_phashsize];
  if(_pwheel == 0 || _pindex == 0)
  {
    cerr << "Error at BNHead. (14)\n";
    exit(1);
  }
  for(int i=0; i<_phashsize; i++) _pwheel[i] = -1;
  for(int i=0; i<n_prb; i++) _pindex[i] = 0;
}

BNNode::~BNNode(void)
{
  for(int i=0; i<n_val; i++) delete[] _vindex[i];
  for(int i=0; i<n_prb; i++) delete[] _pindex[i];
  delete[] _pindex;
  delete[] _pwheel;
  delete[] _nonzero;
  delete[] _vindex;
  delete[] _vwheel;
  delete[] name;
}

void BNNode::SetName(char* nodename)
{
  name = new char[strlen(nodename)+1];
  if(name == 0)
  {
    cerr << "Error at BNNode. (11)\n";
    exit(1);
  }
  strcpy(name, nodename);
}

void BNNode::SetNVal(int n_value)
{
  if(n_val > 0 || n_value <= 0)
  {
    cerr << "Error at BNNode. (1)\n";
    exit(1);
  }
  n_val = n_value;

  for(_vhashsize=16; _vhashsize<(n_val<<1); _vhashsize<<=1)
    ; // empty
  _vwheel = new int[_vhashsize];
  _vindex = new char* [n_val];
  _nonzero = new int[n_val];
  if(_vwheel == 0 || _vindex == 0 || _nonzero == 0)
  {
    cerr << "Error at BNHead. (2)\n";
    exit(1);
  }
  for(int i=0; i<_vhashsize; i++) _vwheel[i] = -1;
  for(int i=0; i<n_val; i++)
  {
    _vindex[i] = 0;
    _nonzero[i] = 0;
  }
}

char* BNNode::GetVal(int index)
{
  if(index < 0 || index >= n_val)
  {
    cerr << "Error at BNHead. (9)\n";
    exit(1);
  }
  return _vindex[index];
}

void BNNode::SetNZ(int index)
{
  if(index < 0 || index >= n_val)
  {
    cerr << "Error at BNHead. (21)\n";
    exit(1);
  }
  _nonzero[index] = 1;
}

int BNNode::ChkNZ(int index)
{
  if(index < 0 || index >= n_val)
  {
    cerr << "Error at BNHead. (20)\n";
    exit(1);
  }
  return _nonzero[index];
}

int BNNode::GetValWheelIndex(char* valname)
{
  if(valname == 0)
  {
    cerr << "Error at BNNode. (3)\n";
    exit(1);
  }
  int hash = 0;
  for(int i=0; i<32; i++)
  {	
    if(valname[i] == 0) break;
    hash = (hash * 123456 + valname[i]) & (_vhashsize - 1);
  }
  int i = hash;
  while(_vwheel[i] != -1)
  {
    if(strcmp(valname, _vindex[_vwheel[i]]) == 0)
      return i;
    i++;
    i &= (_vhashsize -1);
  }
  return i;
}

int BNNode::GetValIndex(char* valname)
{
  return _vwheel[ GetValWheelIndex(valname) ];
}

void BNNode::SetVal(int index, char* valname)
{
  int wix = GetValWheelIndex(valname);
  if(_vwheel[wix] != -1) return;
  _vwheel[wix] = index;
  _vindex[index] = new char[strlen(valname)+1];
  if(_vindex[index] == 0)
  {
    cerr << "Error at BNHead. (12)\n";
    exit(1);
  }
  strcpy(_vindex[index], valname);
}

char* BNNode::GetPrb(int index)
{
  if(index < 0 || index >= n_prb)
  {
    cerr << "Error at BNHead. (9)\n";
    exit(1);
  }
  return _pindex[index];
}

int BNNode::GetPrbWheelIndex(char* prbname)
{
  if(prbname == 0)
  {
    cerr << "Error at BNNode. (4)\n";
    exit(1);
  }
  int hash = 0;
  for(int i=0; i<32; i++)
  {	
    if(prbname[i] == 0) break;
    hash = (hash * 123456 + prbname[i]) & (_phashsize - 1);
  }
  int i = hash;
  while(_pwheel[i] != -1)
  {
    if(strcmp(prbname, _pindex[_pwheel[i]]) == 0)
      return i;
    i++;
    i &= (_phashsize -1);
  }
  return i;
}

int BNNode::GetPrbIndex(char* prbname)
{
  return _pwheel[ GetPrbWheelIndex(prbname) ];
}

void BNNode::EnterPrb(char* prbname)
{
  int wix = GetPrbWheelIndex(prbname);
  if(_pwheel[wix] != -1) return;
  _pwheel[wix] = n_prb;
  _pindex[n_prb] = new char[strlen(prbname)+1];
  if(_pindex[n_prb] == 0)
  {
    cerr << "Error at BNHead. (13)\n";
    exit(1);
  }
  strcpy(_pindex[n_prb], prbname);
  if(++n_prb >= (_phashsize>>1)) EnlargePrb();
}

void BNNode::EnlargePrb(void)
{
  int oldsize = _phashsize;
  int* oldwheel = _pwheel;
  char** oldindex = _pindex;

  _phashsize <<= 2;
  _pwheel = new int[_phashsize];
  _pindex = new char* [_phashsize];
  if(_pwheel == 0 || _pindex == 0)
  {
    cerr << "Error at FuncTable. (15)\n";
    exit(1);
  }
  for(int i=0; i<_phashsize; i++)
  {
    _pwheel[i] = -1;
    _pindex[i] = 0;
  }
  for(int i=0; i<n_prb; i++)
  {
    _pindex[i] = oldindex[i];
    _pwheel[ GetPrbWheelIndex(_pindex[i]) ] = i;
  }
  delete[] oldindex;
  delete[] oldwheel;
}
 
int BNNode::GetFin(int index)
{
  if(index < 0 || index >= n_fin)
  {
    cerr << "Error at BNHead. (8)\n";
    exit(1);
  }
  return _fin[index];
}

void BNNode::EnterFin(int nodeindex)
{
  _fin[n_fin] = nodeindex;
  n_fin++;
}

void BNNode::AllocCPT(void)
{
  if(n_cpt > 0)
  {
    cerr << "Error at BNNode. (5)\n";
    exit(1);
  }

  n_cpt = 1;
  for(int i=n_fin-1; i>=0; i--)
  {
    offset[i] = n_cpt;
    n_cpt *= head->GetNode(GetFin(i))->n_val;
  }
  myoffset = n_cpt;
  n_cpt *= n_val;

  _cpt = new int[n_cpt];
  if(_cpt == 0)
  {
    cerr << "Error at BNNode. (6)\n";
    exit(1);
  }
  for(int i=0; i<n_cpt; i++) _cpt[i] = 0;
}

void BNNode::SetCPT(int index, int prb)
{
  if(index < 0 || index >= n_cpt || prb < 0 || prb>= n_prb)
  {
    cerr << "Error at BNHead. (10)\n";
    exit(1);
  }
  _cpt[index] = prb;
}

int BNNode::GetCPT(int index)
{
  if(index < 0 || index >= n_cpt)
  {
    cerr << "Error at BNHead. (7)\n";
    exit(1);
  }
  return _cpt[index];
}
