/****************************************
 * CtoI Class (SAPPORO-1.55)            *
 * (Graphic methods)                    *
 * (C) Shin-ichi MINATO (Dec. 11, 2012) *
 ****************************************/

#include "CtoI.h"

void CtoI::XPrint() const
{
	int len = TopDigit() + 1;
	bddword* bddv = new bddword[len];
	for(int i=0; i<len; i++) bddv[i] = Digit(i).GetZBDD().GetID(); 
	bddvgraph(bddv, len);
	delete[] bddv;
}

void CtoI::XPrint0() const
{
	int len = TopDigit() + 1;
	bddword* bddv = new bddword[len];
	for(int i=0; i<len; i++) bddv[i] = Digit(i).GetZBDD().GetID(); 
	bddvgraph0(bddv, len);
	delete[] bddv;
}

