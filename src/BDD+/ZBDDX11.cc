/****************************************
 * ZBDD+ Manipulator (SAPPORO-1.55)     *
 * (Graphic methods)                    *
 * (C) Shin-ichi MINATO (Dec. 11, 2012) *
 ****************************************/

#include "ZBDD.h"

void ZBDD::XPrint() const
{
	bddgraph(_zbdd);
}

void ZBDDV::XPrint() const
{
	int len = Last() + 1;
	bddword* bddv = new bddword[len];
	for(int i=0; i<len; i++) bddv[i] = GetZBDD(i).GetID(); 
	bddvgraph(bddv, len);
	delete[] bddv;
}

/*
void ZBDD::XPrint0()
{
	bddgraph0(_zbdd);
}

void ZBDDV::XPrint0()
{
	int len = Last() + 1;
	bddword* bddv = new bddword[len];
	for(int i=0; i<len; i++) bddv[i] = GetZBDD(i).GetID(); 
	bddvgraph0(bddv, len);
	delete[] bddv;
}
*/
