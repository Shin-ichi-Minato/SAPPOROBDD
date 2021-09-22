/****************************************
 * BDD+ Manipulator (SAPPORO-1.55)      *
 * (Graphic methods)                    *
 * (C) Shin-ichi MINATO (Dec. 11, 2012) *
 ****************************************/

#include "BDD.h"

void BDD::XPrint0() const
{
	bddgraph0(_bdd);
}

void BDDV::XPrint0() const
{
	bddword* bddv = new bddword[_len];
	for(int i=0; i<_len; i++) bddv[i] = GetBDD(i).GetID(); 
	bddvgraph0(bddv, _len);
	delete[] bddv;
}

void BDD::XPrint() const
{
	bddgraph(_bdd);
}

void BDDV::XPrint() const
{
	bddword* bddv = new bddword[_len];
	for(int i=0; i<_len; i++) bddv[i] = GetBDD(i).GetID(); 
	bddvgraph(bddv, _len);
	delete[] bddv;
}

