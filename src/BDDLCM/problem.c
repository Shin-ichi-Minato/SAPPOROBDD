/*  Common problem input/output routines /structure
            25/Nov/2007   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users.
   For the commercial use, please make a contact to Takeaki Uno. */

/***************************************************/

#ifndef _problem_c_
#define _problem_c_

#include"problem.h"

#include"stdlib2.c"
#include"queue.c"
#include"itemset.c"

void PROBLEM_error (){
  ERROR_MES = "command explanation";
  EXIT;
}

/*************************************************************************/
/* PROBLEM and ITEMSET initialization */
/*************************************************************************/
void PROBLEM_init (PROBLEM *P){
  P->start_time = clock();
  RAND_INIT;
  P->problem = 0;
  P->prog = 0;
  P->prog2 = 0;
  P->input_fname = P->output_fname = P->weight_fname = NULL;
  P->table_fname = P->position_fname = NULL;

  P->sgraph_fname = P->sgraph2_fname = NULL;
  P->sgraph_wfname = P->sgraph2_wfname = NULL;
  P->agraph_fname = P->agraph2_fname = NULL;
  P->trsact_fname = P->trsact2_fname = NULL;
  P->trsact_fname2 = P->trsact2_fname2 = NULL;
  P->trsact_wfname = P->trsact2_wfname = NULL;
  P->trsact_wfname2 = P->trsact2_wfname2 = NULL;
  P->trsact_pfname = P->trsact2_pfname = NULL;
  P->seq_fname = P->seq2_fname = NULL;
  P->fstar_fname = P->fstar2_fname = NULL;
  P->mat_fname = P->mat2_fname = NULL;
  P->smat_fname = P->smat2_fname = NULL;
  P->setfamily_fname = P->setfamily2_fname = NULL;
  P->setfamily_wfname = P->setfamily2_wfname = NULL;

  P->root = 0;
  P->dir = P->edge_dir = 0;
  P->th = P->th2 = P->th3 = 0;
  P->ratio = P->ratio2 = 0;
  P->num = P->siz = P->dim = P->len = 0;
  P->rows = 0;
  P->clms = 0;
  
  ITEMSET_init (&P->II);
  ITEMSET_init (&P->II2);

  P->vf = P->dep = NULL;
  P->ff = INIT_QUEUE;

  P->shift = NULL;
  P->occ_w = P->occ_pw = P->occ_w2 = P->occ_pw2 = NULL;

  P->itemjump = P->itemcand = P->vecjump = P->veccand = INIT_QUEUE;  // for delivery
  P->OQ = P->OQ2 = P->VQ = P->VQ2 = NULL;   // for delivery
  P->itemary = NULL;
  P->itemmark = P->itemflag = P->vecmark = P->vecflag = NULL;  // mark for vector
  P->occ_t = P->vecary = NULL;
  P->oo = INIT_QUEUE;
  
  P->pat = NULL;
  P->plen = P->perr = 0;
  
#ifdef _alist_h_
  P->occ = INIT_MALIST;
#endif

#ifdef _trsact_h_
  TRSACT_init (&P->TT);
  TRSACT_init (&P->TT2);
#endif
#ifdef _sgraph_h_
  P->SG = INIT_SGRAPH;
  P->SG2 = INIT_SGRAPH;
#endif
#ifdef _agraph_h_
  P->AG = INIT_AGRAPH;
  P->AG2 = INIT_AGRAPH;
#endif
#ifdef _seq_h_
  SEQ_init (&P->SS);
  SEQ_init (&P->SS2);
#endif
#ifdef _fstar_h_
  P->FS = INIT_FSTAR;
  P->FS2 = INIT_FSTAR;
#endif

#ifdef _vec_h_
  P->MM = INIT_MAT;
  P->MM2 = INIT_MAT;
  P->SM = INIT_SMAT;
  P->SM2 = INIT_SMAT;
  P->FF = INIT_SETFAMILY;
  P->FF2 = INIT_SETFAMILY;
#endif
}

/*************************************************************************/
/* PROBLEM initialization */
/* all pointers are set to NULL, but don't touch filenames  */
/* load_flag, flag to give TRSACT_load */
/* II->item_max will be item_max when do not load problem */
/* II-> */
/*************************************************************************/
void PROBLEM_init2 (PROBLEM *P, int flag){
  int f=0;
/******************************/
#ifdef _trsact_h_
  if ( P->trsact_fname ){
    TRSACT_load (&P->TT, P->trsact_fname, P->trsact_fname2, P->trsact_wfname, P->trsact_wfname2, P->trsact_pfname);       if (ERROR_MES) goto ERR;
    if ( P->TT.flag & SHOW_MESSAGE ){
      print_err ("trsact: %s", P->trsact_fname);
      if ( P->trsact2_fname2 ) print_err (" ,2nd-trsact2 %s (from ID %d)", P->trsact_fname2, P->TT.end1);
      print_err (" ,#transactions %d ,#items %d ,size %d", P->TT.rows_org, P->TT.clms_org, P->TT.eles_org);
      print_err (" extracted database: #transactions %d ,#items %d ,size %zd", P->TT.T.t, P->TT.T.clms, P->TT.T.eles);
      if ( P->trsact_wfname ) print_err (" ,weightfile %s", P->trsact_wfname);
      if ( P->trsact_wfname2 ) print_err (" ,2nd-weightfile %s", P->trsact_wfname2);
      if ( P->trsact_pfname ) print_err (" ,item-order-file %s", P->trsact_pfname);
      print_err ("\n");
    }
  }
  if ( P->trsact2_fname ){
    TRSACT_load (&P->TT2, P->trsact2_fname, P->trsact2_fname2, P->trsact2_wfname, P->trsact2_wfname2, P->trsact2_pfname);       if (ERROR_MES) goto ERR;
    if ( P->TT2.flag & SHOW_MESSAGE ){
      print_err ("trsact2: %s", P->trsact2_fname);
      if ( P->trsact2_fname2 ) print_err (" ,2nd-trsact2 %s (from ID %d)", P->trsact2_fname2, P->TT.end1);
      print_err (" ,#transactions %d ,#items %d ,size %zd", P->TT2.rows_org, P->TT2.clms_org, P->TT2.eles_org);
      print_err (" extracted database: #transactions %d ,#items %d ,size %zd", P->TT2.T.t, P->TT2.T.clms, P->TT2.T.eles);
      if ( P->trsact2_wfname ) print_err (" ,weightfile2 %s", P->trsact2_wfname);
      if ( P->trsact2_wfname2 ) print_err (" ,2nd-weightfile2 %s", P->trsact2_wfname2);
      if ( P->trsact2_pfname ) print_err (" ,item-order-file2 %s", P->trsact2_pfname);
      print_err ("\n");
    }
  }
#endif
#ifdef _sgraph_h_
  if ( P->sgraph_fname ){
    SGRAPH_load (&P->SG, P->sgraph_fname, P->sgraph_wfname);    if (ERROR_MES) goto ERR;
    print_mes (P->SG.flag, "sgraph: %s ,#nodes %d ,#edges %zd ,#arcs %zd\n", P->sgraph_fname, SGRAPH_NODE_NUM(P->SG), P->SG.edge.eles/2,  P->SG.in.eles);
  }
  if ( P->sgraph2_fname ){
    SGRAPH_load (&P->SG, P->sgraph2_fname, P->sgraph2_wfname);    if (ERROR_MES) goto ERR;
    print_mes (P->SG2.flag, "sgraph: %s ,#nodes %d ,#edges %zd ,#arcs %zd\n", P->sgraph2_fname, SGRAPH_NODE_NUM(P->SG2), P->SG2.edge.eles/2, P->SG2.in.eles);
  }
#endif
#ifdef _agraph_h_
  if ( P->agraph_fname ){
    AGRAPH_load (&P->AG, P->agraph_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->AG.flag, "agraph: %s ,#nodes %d ,#edges %zd ,#arcs %zd\n", P->agraph_fname, P->AG.node_num, P->AG.edge_num,  P->AG.arc_num);
  }
  if ( P->agraph2_fname ){
    AGRAPH_load (&P->AG2, P->agraph_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->AG2.flag, "agraph: %s ,#nodes %d ,#edges %zd ,#arcs %zd\n", P->agraph2_fname, P->AG2.node_num, P->AG2.edge_num,  P->AG2.arc_num);
  }
#endif
#ifdef _fstar_h_
  if ( P->fstar_fname ){
    FSTAR_load (&P->FS, P->fstar_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->FS.flag, "agraph: %s ,#nodes %d(%d,%d) ,#edges %zd\n", P->fstar_fname, P->FS.node_num, P->FS.in_node_num, P->FS.out_node_num, P->FS.edge_num);
  }
  if ( P->fstar2_fname ){
    FSTAR_load (&P->FS2, P->fstar2_fname);     if (ERROR_MES) goto ERR;
    print_mes (P->FS2.flag, "agraph2: %s ,#nodes %d(%d,%d) ,#edges %zd\n", P->fstar2_fname, P->FS2.node_num, P->FS2.in_node_num, P->FS2.out_node_num, P->FS2.edge_num);
  }

#endif
#ifdef _vec_h_
  if ( P->mat_fname ){
    MAT_load (&P->MM, P->mat_fname);   if (ERROR_MES) goto ERR;
    print_mes (P->MM.flag, "mat: %s ,#rows %d ,#clms %d\n", P->mat_fname, P->MM.t, P->MM.clms);
  }
  if ( P->mat2_fname ){
    MAT_load (&P->MM2, P->mat2_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->MM2.flag, "mat2: %s ,#rows %d ,#clms %d\n", P->mat2_fname, P->MM2.t, P->MM2.clms);
  }
  if ( P->smat_fname ){
    SMAT_load (&P->SM, P->smat_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->SM.flag, "smat: %s ,#rows %d ,#clms %d ,#eles %zd\n", P->smat_fname, P->SM.t, P->SM.clms, P->SM.eles);
  }
  if ( P->smat2_fname ){
    SMAT_load (&P->SM2, P->smat2_fname);    if (ERROR_MES) goto ERR;
    print_mes (P->SM2.flag, "smat2: %s ,#rows %d ,#clms %d ,#eles %zd\n", P->smat2_fname, P->SM2.t, P->SM2.clms, P->SM2.eles);
  }
  if ( P->setfamily_fname ){
    SETFAMILY_load (&P->FF, P->setfamily_fname, P->setfamily_wfname);   if (ERROR_MES) goto ERR;
    print_mes (P->FF.flag, "setfamily: %s ,#rows %d ,#clms %d ,#eles %zd", P->setfamily_fname, P->FF.t, P->FF.clms, P->FF.eles);
    if ( P->setfamily_wfname ) print_mes (P->FF.flag, " ,weightfile %s", P->setfamily_wfname);
    print_mes (P->FF.flag, "\n");
  }
  if ( P->setfamily2_fname ){
    SETFAMILY_load (&P->FF2, P->setfamily2_fname, P->setfamily2_wfname);   if (ERROR_MES) goto ERR;
    print_mes (P->FF2.flag, "setfamily2: %s ,#rows %d ,#clms %d ,#eles %zd", P->setfamily2_fname, P->FF2.t, P->FF2.clms, P->FF2.eles);
    if ( P->setfamily2_wfname ) print_mes (P->FF2.flag, " ,weightfile %s", P->setfamily2_wfname);
    print_mes (P->FF2.flag, "\n");
  }
#endif
  if (P->input_fname){ f=1; print_err (" input: %s", P->input_fname); }
  if (P->weight_fname){ f=1; print_err (" weight: %s", P->weight_fname); }
  if (P->output_fname){ f=1;  print_err (" output to: %s",P->output_fname); }
  if ( f )print_err ("\n");

/******************************/

  if ( flag&SHOW_MESSAGE ){
    ITEMSET_print (&P->II, (flag&PROBLEM_PRINT_FRQ)? 1: 0);
    if ( flag&PROBLEM_PRINT_DENSE ){
      print_err ("density threshold");
      fprint_real (stderr, P->dense);
      print_err ("\n");
    }
  }

  if ( !ERROR_MES ) return;
  ERR:;
  PROBLEM_end (P);
  EXIT;
}

/* termination of problem */
void PROBLEM_end (PROBLEM *P){
  ITEMSET *II = &P->II;

#ifdef _trsact_h_
  TRSACT_end (&P->TT);
  TRSACT_end (&P->TT2);
#endif
#ifdef _sgraph_h_
  SGRAPH_end (&P->SG);
  SGRAPH_end (&P->SG2);
#endif
#ifdef _agraph_h_
  AGRAPH_end (&P->AG);
  AGRAPH_end (&P->AG2);
#endif
#ifdef _seq_h_
  SEQ_end (&P->SS);
  SEQ_end (&P->SS2);
#endif
#ifdef _fstar_h_
  FSTAR_end (&P->FS);
  FSTAR_end (&P->FS2);
#endif
#ifdef _vec_h_
  MAT_end (&P->MM);
  MAT_end (&P->MM2);
  SMAT_end (&P->SM);
  SMAT_end (&P->SM2);
  SETFAMILY_end (&P->FF);
  SETFAMILY_end (&P->FF2);
#endif

/******************************/

  mfree (P->vf, P->dep);
  QUEUE_end (&P->ff);

  ITEMSET_end (II);
  ITEMSET_end (&P->II2);

  if ( P->occ_pw2 != P->occ_pw && P->occ_pw2 != P->occ_w2 ) free2 (P->occ_pw2);
  if ( P->occ_w2 != P->occ_w ) free2 (P->occ_w2);
  if ( P->occ_pw != P->occ_w ) free2 (P->occ_pw);
  mfree (P->shift, P->occ_t, P->occ_w);

  if ( P->OQ ) free2 (P->OQ[0].v);
  if ( P->OQ2 ) free2 (P->OQ2[0].v);
  if ( P->VQ ) free2 (P->VQ[0].v);
  if ( P->VQ2 ) free2 (P->VQ2[0].v);
  mfree (P->OQ, P->OQ2, P->VQ, P->VQ2);


  mfree (P->itemary, P->itemflag, P->itemmark, P->vecary, P->vecflag, P->vecmark);
  QUEUE_end (&P->itemcand);
  QUEUE_end (&P->itemjump);

  QUEUE_end (&P->veccand);
  QUEUE_end (&P->vecjump);
  QUEUE_end (&P->oo);


#ifdef _alist_h_
  MALIST_end (&P->occ);
#endif

#ifdef _undo_h_
  ALISTundo_end ();
#endif

  P->end_time = clock();
  if ( print_time_flag )
    print_err ("computation_time= %3f\n", ((double)(P->end_time - P->start_time))/CLOCKS_PER_SEC);

  PROBLEM_init (P);
}

/* allocate arrays and structures */
void PROBLEM_alloc (PROBLEM *P, QUEUE_ID siz, QUEUE_ID siz2, size_t siz3, PERM *perm, int f){
#ifdef _alist_h_
  ALIST_ID i;
#endif

  if ( f&PROBLEM_SHIFT ) calloc2 (P->shift, siz+2, "PROBLEM_alloc: shift", goto ERR);
  if ( f&PROBLEM_OCC_T ) calloc2 (P->occ_t, siz+2, "PROBLEM_alloc:occ_t", goto ERR);
  if ( f&(PROBLEM_OCC_W+PROBLEM_OCC_PW) ) calloc2 (P->occ_w, siz+2, "PROBLEM_alloc:occ_w", goto ERR);
  if ( f&PROBLEM_OCC_PW ){
    calloc2 (P->occ_pw, siz+2, "PROBLEM_alloc:occ_pw", goto ERR);
  } else P->occ_pw = P->occ_w;
  if ( f&PROBLEM_OCC_W2 ){
    calloc2 (P->occ_w2, siz+2, "PROBLEM_alloc:occ_w", goto ERR);
    if ( f&PROBLEM_OCC_PW ){
      calloc2 (P->occ_pw2, siz+2, "PROBLEM_alloc:occ_pw", goto ERR);
    } else P->occ_pw2 = P->occ_w2;
  } else { P->occ_w2 = P->occ_w; P->occ_pw2 = P->occ_pw; }

  if ( f&PROBLEM_ITEMFLAG ) calloc2 (P->itemflag, siz+2, "PROBLEM_alloc:itemflag", goto ERR);
  if ( f&PROBLEM_ITEMMARK ) calloc2 (P->itemmark, siz+2, "PROBLEM_alloc:itemmark", goto ERR);
  if ( f&PROBLEM_ITEMARY ) calloc2(P->itemary, siz+2,"PROBLEM_alloc:itemary", goto ERR);
  if ( f&PROBLEM_ITEMJUMP ) QUEUE_alloc (&P->itemjump, siz+2);
  if ( f&PROBLEM_ITEMCAND ) QUEUE_alloc (&P->itemcand, siz+2);

  if ( f&PROBLEM_VECFLAG ) calloc2 (P->vecflag, siz+2, "PROBLEM_alloc:vecflag", goto ERR);
  if ( f&PROBLEM_VECMARK ) calloc2 (P->vecmark, siz2+2, "PROBLEM_alloc:vecmark", goto ERR);
  if ( f&PROBLEM_VECARY ) calloc2 (P->vecary, siz2+2, "PROBLEM_alloc:vecary", goto ERR);
  if ( f&PROBLEM_VECJUMP ) QUEUE_alloc (&P->vecjump, siz2+2);
  if ( f&PROBLEM_VECCAND ) QUEUE_alloc (&P->veccand, siz2+2);

#ifdef _alist_h_
  if ( f&PROBLEM_OCC3){
    MALIST_alloc (&P->occ, siz, siz2+2); // element=>
if ( ERROR_MES ) goto ERR;
    if ( f&PROBLEM_OCC2 )
      FLOOP (i, 0, siz) MALIST_ins_tail (&P->occ, (f&PROBLEM_OCC1)?siz: 0, i, 0);
  }
#endif

  ITEMSET_init2 (&P->II, P->output_fname, perm, siz, siz3);
  if ( P->II.target<siz && P->II.perm ) P->II.target = P->II.perm[P->II.target];

#ifdef _undo_h_
  ALISTundo_init ();
#endif

  return;
  ERR:;
  PROBLEM_end (P);
  EXIT;
}

#endif


