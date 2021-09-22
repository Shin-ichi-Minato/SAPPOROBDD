/#define _lcm_c_/a #include "CtoI.h" //ZDD \

/void LCM (/c CtoI LCM (PROBLEM *PP, int item, QUEUE *occ, WEIGHT frq, WEIGHT pfrq){ //ZDD \
  int ii, xx; //ZDD \
  CtoI F, G; //ZDD \
  F = CtoI(0); //ZDD

/ITEMSET_check_all_rule (/a \      F = (II->flag & ITEMSET_PRE_FREQ)? CtoI((int)II->frq): CtoI(1); //ZDD \
      for(ii=0; ii<II->add.t; ii++) { //ZDD \
        xx = II->add.v[ii]; //ZDD \
	G = F.AffixVar(BDD_VarOfLev(xx+1)); //ZDD \
	if(PP->problem & PROBLEM_FREQSET) { //ZDD \
          F = CtoI_Union(F, G); //ZDD \
	} //ZDD \
	else F = G; //ZDD \
      } //ZDD 

/LCM (PP, e/c \      G = LCM (PP, e, &TT->OQ[e], PP->occ_w2[e], PP->occ_pw2[e]); // recursive call //ZDD \
      F = CtoI_Union(F, G); //ZDD

/END:;/a \  if(item < II->item_max) { //ZDD \
    xx = item; //ZDD \
    F = F.AffixVar(BDD_VarOfLev(xx+1)); //ZDD \
  } //ZDD

/PP->itemcand.s = js;/a \  return F; //ZDD
/int LCM_main/c extern PROBLEM LCM_PP; //ZDD \
PROBLEM LCM_PP; //ZDD \
extern CtoI CtoI_Lcm2(); //ZDD \
 \
int CtoI_LcmItems() { //ZDD \
  if( ERROR_MES ) return -1; //ZDD \
  return LCM_PP.II.item_max; //ZDD \
} //ZDD \
 \
int CtoI_LcmPerm(int k) { //ZDD \
  if( ERROR_MES ) return -1; //ZDD \
  return LCM_PP.II.perm[k]; //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmA ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 0); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmC ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 1); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmM ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 2); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmAV ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 10); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmCV ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 11); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
CtoI CtoI_LcmMV ( char *fname1, char *fname2, int th){ //ZDD \
  CtoI_Lcm1(fname1, fname2, th, 12); //ZDD \
  if( !ERROR_MES ) while(CtoI_LcmItems() > BDD_VarUsed()) BDD_NewVar(); //ZDD \
  return CtoI_Lcm2(); //ZDD \
} //ZDD \
 \
int CtoI_Lcm1 ( char *fname1, char *fname2, int th, int param ) { //ZDD

/PROBLEM PP/d

/ITEMSET \*II = &PP\./c \  ITEMSET *II = &LCM_PP.II; //ZDD

/TRSACT \*TT = &PP\./c \  TRSACT *TT = &LCM_PP.TT; //ZDD

/SGRAPH \*SG = &PP\./c \  SGRAPH *SG = &LCM_PP.SG; //ZDD

/PROBLEM_init (/c \  ERROR_MES = 0; //ZDD \
  PROBLEM_init ( &LCM_PP ); //ZDD 

/LCM_read_param (argc/c \  switch(param) //ZDD \
  { //ZDD \
  case 12: //ZDD \
    LCM_PP.problem |= PROBLEM_MAXIMAL; //ZDD \
    LCM_PP.TT.flag |= TRSACT_UNION; //ZDD \
    II->flag |= ITEMSET_PRE_FREQ; //ZDD \
    break; //ZDD \
  case 11: //ZDD \
    LCM_PP.problem |= PROBLEM_CLOSED; //ZDD \
    LCM_PP.TT.flag |= TRSACT_INTSEC; //ZDD \
    II->flag |= ITEMSET_PRE_FREQ; //ZDD \
    break; //ZDD \
  case 10: //ZDD \
    LCM_PP.problem |= PROBLEM_FREQSET; //ZDD \
    II->flag |= ITEMSET_PRE_FREQ; //ZDD \
    II->flag |= ITEMSET_ALL; //ZDD \
    break; //ZDD \
  case 2: //ZDD \
    LCM_PP.problem |= PROBLEM_MAXIMAL; //ZDD \
    LCM_PP.TT.flag |= TRSACT_UNION; //ZDD \
    break; //ZDD \
  case 1: //ZDD \
    LCM_PP.problem |= PROBLEM_CLOSED; //ZDD \
    LCM_PP.TT.flag |= TRSACT_INTSEC; //ZDD \
    break; //ZDD \
  case 0: //ZDD \
  default: //ZDD \
    LCM_PP.problem |= PROBLEM_FREQSET; //ZDD \
    II->flag |= ITEMSET_ALL; //ZDD \
  } //ZDD \
  LCM_PP.trsact_fname = fname1; //ZDD \
  LCM_PP.trsact_pfname = fname2; //ZDD \
  II->frq_lb = (WEIGHT)th; //ZDD

/PROBLEM_init2 (/c \  PROBLEM_init2 ( &LCM_PP, PROBLEM_PRINT_SHRINK + PROBLEM_PRINT_FRQ ); //ZDD 

/LCM_init (&PP)/c \    LCM_init(&LCM_PP); //ZDD \
  } //ZDD \
  return 0; //ZDD \
} //ZDD \
 \
CtoI CtoI_Lcm2 () { //ZDD \
  CtoI F, G; //ZDD \
  if ( ERROR_MES ) { //ZDD \
    PROBLEM_end( &LCM_PP ); //ZDD \
    return CtoI(-1); //ZDD \
  } //ZDD \
  else //ZDD \
  { //ZDD \
    F = CtoI(0); //ZDD

/if ( ERROR_MES ) return;/c if ( ERROR_MES ) return CtoI(-1); //ZDD
/LCM (&PP, TT->T\.clms/c \    if ( !ERROR_MES ) G = LCM (&LCM_PP, LCM_PP.TT.T.clms, &LCM_PP.oo, LCM_PP.TT.total_w_org, LCM_PP.TT.total_pw_org); //ZDD \
    else G = CtoI(0); //ZDD \
    F = CtoI_Union(F, G); //ZDD

/ITEMSET_last/c \    ITEMSET_last_output (&LCM_PP.II); //ZDD

/TT->sc = NULL/c \  LCM_PP.TT.sc = NULL; //ZDD

/PROBLEM_end (&PP)/c \  PROBLEM_end (&LCM_PP); //ZDD

/return (ERR/c \  return F; //ZDD

