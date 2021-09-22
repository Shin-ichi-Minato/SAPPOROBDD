/* Linear time Closed itemset Miner for Frequent Itemset Mining problems */
/* 2004/4/10 Takeaki Uno,   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about LCM for the users.
   For the commercial use, please make a contact to Takeaki Uno. */


#ifndef _lcm_c_
#define _lcm_c_

#define WEIGHT_DOUBLE
#define TRSACT_DEFAULT_WEIGHT 1

#define LCM_UNCONST 16777216   // use the complement graph of the constraint graph
#define LCM_POSI_EQUISUPP 33554432   // an item will be dealt as "equisupp" when "positive"-frequency is equal to the positive-frequency of the current itemset 

#define ERROR_RET 

#include"trsact.c"
#include"sgraph.c"
#include"problem.c"

void LCM_error (){
  ERROR_MES = "command explanation";
  print_err ("LCM: [FCMfQIq] [options] input-filename support [output-filename]\n\
F:frequent itemset mining, C:closed frequent itemset mining\n\
M:maximal frequent itemset mining, P:positive-closed itemset mining\n\
f:output frequency following to each output itemset\n\
A:output positive/negative frequency, and their ratio\n\
Q:output frequency and coverages preceding to itemsets\n\
q:no output to standard output, V:show progress of computation\n\
I:output ID's of transactions including each pattern\n\
i:do not output itemset to the output file (only rules)\n\
s:output confidence and item frequency by absolute values\n\
t:transpose the input database (item i will be i-th transaction, and i-th transaction will be item i)\n\
[options]\n\
-K [num]:output [num] most frequent itemsets\n\
-l,-u [num]:output itemsets with size at least/most [num]\n\
-U [num]:upper bound for support(maximum support)\n\
-w [filename]:read weights of transactions from the file\n\
-c,-C [filename]:read item constraint/un-constraint file\n\
-S [num]:stop aftre outputting [num] solutions\n\
-i [num]: find association rule for item [num]\n\
-a,-A [ratio]: find association rules of confidence at least/most [ratio]\n\
-r,-R [ratio]: find association rules of relational confidence at least/most [ratio]\n\
-f,F [ratio]: output itemsets with frequency no less/greater than [ratio] times the frequency given by product of the probability of each item appearance\n\
-p,-P [num]: output itemset only if (frequency)/(abusolute frequency) is no less/no greater than [num]\n\
-n,-N [num]: output itemset only if its negative frequency is no less/no greater than [num] (negative frequency is the sum of weights of transactions having negative weights)\n\
-o,-O [num]: output itemset only if its positive frequency is no less/no greater than [num] (positive frequency is the sum of weights of transactions having positive weights)\n\
-m,-M [filename]:read/write item permutation from/to file [filename]\n\
if the 1st letter of input-filename is '-', be considered as 'parameter list'\n");
  EXIT;
}

/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void LCM_read_param (int argc, char *argv[], PROBLEM *PP){
  ITEMSET *II = &PP->II;
  int c=1, f=0;
  if ( argc < c+3 ){ LCM_error (); return; }
  
  if ( !strchr (argv[c], 'q') ){ II->flag |= SHOW_MESSAGE; PP->TT.flag |= SHOW_MESSAGE; }
  if ( strchr (argv[c], 'f') ) II->flag |= ITEMSET_FREQ;
  if ( strchr (argv[c], 'Q') ) II->flag |= ITEMSET_PRE_FREQ;
  if ( strchr (argv[c], 'A') ) II->flag |= ITEMSET_OUTPUT_POSINEGA;
  if ( strchr (argv[c], 'C') ){ PP->problem |= PROBLEM_CLOSED; PP->TT.flag |= TRSACT_INTSEC; }
  else if ( strchr (argv[c], 'F') ){ PP->problem |= PROBLEM_FREQSET; II->flag |= ITEMSET_ALL; }
  else if ( strchr (argv[c], 'M') ){ PP->problem |= PROBLEM_MAXIMAL; PP->TT.flag |= TRSACT_UNION; }
  else error ("one of F, C, M has to be given", EXIT);
  if ( strchr (argv[c], 'P') ) PP->problem |= LCM_POSI_EQUISUPP;
  if ( strchr (argv[c], 'V') ) II->flag |= SHOW_PROGRESS;
  if ( strchr (argv[c], 'I') ) II->flag |= ITEMSET_TRSACT_ID;
  if ( strchr (argv[c], 'i') ) II->flag |= ITEMSET_NOT_ITEMSET;
  if ( strchr (argv[c], 's') ) II->flag |= ITEMSET_RULE_SUPP;
  if ( strchr (argv[c], 't') ) PP->TT.flag |= LOAD_TPOSE;
  c++;
  
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 'K': if ( PP->problem & PROBLEM_MAXIMAL )
          error ("M command and -K option can not be given simltaneously", EXIT);
        II->topk.end = atoi (argv[c+1]);
      break; case 'm': PP->trsact_pfname = argv[c+1];
      break; case 'M': PP->trsact_pfname = argv[c+1]; PP->TT.flag |= TRSACT_WRITE_PERM;
      break; case 'l': II->lb = atoi (argv[c+1]);
      break; case 'u': II->ub = atoi(argv[c+1]);
      break; case 'U': II->frq_ub = (WEIGHT)atof(argv[c+1]);
      break; case 'w': PP->trsact_wfname = argv[c+1];
      break; case 'c': PP->sgraph_fname = argv[c+1];
      break; case 'C': PP->sgraph_fname = argv[c+1]; PP->problem |= LCM_UNCONST;
      break; case 'S': II->max_solutions = atoi(argv[c+1]);
      break; case 'f': II->prob_lb = atof(argv[c+1]); II->flag |= ITEMSET_RFRQ; f++;
      break; case 'F': II->prob_ub = atof(argv[c+1]); II->flag |= ITEMSET_RINFRQ; f++;
      break; case 'i': II->target = atoi(argv[c+1]);
      break; case 'a': II->ratio_lb = atof(argv[c+1]); II->flag |= ITEMSET_RULE_FRQ; f|=1;
      break; case 'A': II->ratio_ub = atof(argv[c+1]); II->flag |= ITEMSET_RULE_INFRQ; f|=1;
      break; case 'r': II->ratio_lb = atof(argv[c+1]); II->flag |= ITEMSET_RULE_RFRQ; f|=2;
      break; case 'R': II->ratio_ub = atof(argv[c+1]); II->flag |= ITEMSET_RULE_RINFRQ; f|=2;
      break; case 'P': II->flag |= ITEMSET_POSI_RATIO; II->flag |= ITEMSET_IGNORE_BOUND; II->rposi_ub = atof(argv[c+1]); f|=4;
      break; case 'p': II->flag |= ITEMSET_POSI_RATIO; II->flag |= ITEMSET_IGNORE_BOUND; II->rposi_lb = atof(argv[c+1]); f|=4;
      break; case 'n': II->nega_lb = atof(argv[c+1]);
      break; case 'N': II->nega_ub = atof(argv[c+1]);
      break; case 'o': II->posi_lb = atof(argv[c+1]);
      break; case 'O': II->posi_ub = atof(argv[c+1]);
      break; default: goto NEXT;
    }
    c += 2;
    if ( argc < c+2 ){ LCM_error (); return; }
  }

  NEXT:;
  if ( (f&3)==3 || (f&5)==5 || (f&6)==6 ) error ("-f, -F, -a, -A, -p, -P, -r and -R can not specified simultaneously", EXIT); 
  if ( f && (II->flag & ITEMSET_PRE_FREQ) ) BITRM (II->flag, ITEMSET_PRE_FREQ);

  if ( ( PP->problem & PROBLEM_CLOSED ) && PP->sgraph_fname )
      error ("closed itemset mining does not work with item constraints", EXIT);

  if ( (PP->problem & PROBLEM_FREQSET) && (II->flag & (ITEMSET_RULE + ITEMSET_RFRQ + ITEMSET_RINFRQ)) ){
    PP->problem |= PROBLEM_CLOSED; BITRM (PP->problem, PROBLEM_FREQSET);
    BITRM (II->flag, ITEMSET_ALL);
  }
  PP->trsact_fname = argv[c];
  if ( II->topk.end==0 ) II->frq_lb = (WEIGHT)atof(argv[c+1]);
  if ( argc>c+2 ) PP->output_fname = argv[c+2];
}

/*********************************************************************/
/* add an item to itemset, and update data */
/*********************************************************************/
void LCM_add_item (PROBLEM *PP, QUEUE *Q, QUEUE_INT item){
  QUEUE_INT *x;
  ARY_INS (*Q, item);
  PP->II.itemflag[item] = 1;
  if ( PP->sgraph_fname )
      MQUE_MLOOP (PP->SG.edge.v[item], x, item) PP->itemary[*x]++;
}

/*********************************************************************/
/* delete an item from itemset, and update data */
/*********************************************************************/
void LCM_del_item (PROBLEM *PP, QUEUE *Q){
  QUEUE_INT *x, item = Q->v[--Q->t];
  PP->II.itemflag[item] = 0;
  if ( PP->sgraph_fname )
      MQUE_MLOOP (PP->SG.edge.v[item], x, item)  PP->itemary[*x]--;
}

/* remove unnecessary transactions which do not include all posi_closed items */
/* scan of each transaction is up to item */
void LCM_reduce_occ_by_posi_equisupp (PROBLEM *PP, QUEUE *occ, QUEUE_INT item, QUEUE_INT full){
  QUEUE_ID ii=0;
  TRSACT *TT = &PP->TT;
  ITEMSET *II = &PP->II;
  QUEUE_INT *x, *y, *z, cnt;

  MQUE_FLOOP (*occ, x){
    if ( TT->w[*x]>= 0 ) continue;
    cnt = 0;
    MQUE_MLOOP (TT->T.v[*x], y, item) if ( II->itemflag[*y] == 2 ) cnt++;
    if ( cnt==full ) occ->v[ii++] = *x;
    else {
      II->frq -= TT->w[*x];
      MQUE_MLOOP (TT->T.v[*x], z, item) PP->occ_w[*z] -= TT->w[*x];
    }
  }
  occ->t = ii;
  MQUE_FLOOP (PP->itemcand, x){
    if ( II->itemflag[*x] == 2 ) II->itemflag[*x] = 1;
  }
}

/*************************************************************************/
/* ppc check and maximality check */
/* INPUT: O:occurrence, jump:items, th:support, frq:frequency, add:itemset
   OUTPUT: maximum item i s.t. frq(i)=frq
   OPERATION: remove infrequent items from jump, and 
    insert items i to "add" s.t. frq(i)=frq                              */
/*************************************************************************/
/* functions
  1. when closed itemset mining or maximal frequent itemset mining, find all items
   included in all transactions in occ (checked by pfrq, occ_w
   if there is such an item with index>item, ppc condition is violated, and return non-negative value
  2. when constraint graph is given, set the frequency (occ_w) of the items which can
   not be added to itemset to infrequent number.
  3. count the size of reduced database
  4. call LCM_reduce_occ_posi
 */
QUEUE_INT LCM_maximality_check (PROBLEM *PP, QUEUE *occ, QUEUE_INT item, QUEUE_INT *fmax, QUEUE_INT *cnt){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE_INT m = TT->T.clms, full=0, *x;
  WEIGHT w=-WEIGHTHUGE;
  *fmax = TT->T.clms; *cnt=0;

  MQUE_FLOOP (TT->jump, x){
    if ( II->itemflag[*x] == 1) continue;
//QUEUE_perm_print (&II->itemset, II->perm);
    if ( PP->sgraph_fname && ( (((PP->problem & LCM_UNCONST)==0) && (PP->itemary[*x]>0) ) || 
        ((PP->problem & LCM_UNCONST) && (PP->itemary[*x]<II->itemset.t ))) ){
      // e can not be added by item constraint
//      PP->occ_pw[e] = PP->occ_w[e] = II->frq_lb -1;
      II->itemflag[*x] = 3;
    } else if ( ISEQUAL(PP->occ_pw[*x],II->pfrq) && ( ISEQUAL(PP->occ_w[*x],II->frq) || (PP->problem & LCM_POSI_EQUISUPP) ) ){ // check e is included in all transactions in occ
      if ( *x<item ){
        if ( !PP->sgraph_fname ){ // add item as "equisupport"
          LCM_add_item (PP, &II->add, *x);
          if ( (PP->problem&LCM_POSI_EQUISUPP) && (II->flag&ITEMSET_RULE) ) II->itemflag[*x] = 0; // in POSI_EQUISUPP, occ_w[*x] is not equal to II->frq, thus we have to deal it in the rule mining
        }
        if ( !ISEQUAL(PP->occ_w[*x],II->frq) ){ full++; II->itemflag[*x] = 2; }
      } else m = *x; // an item in prefix can be added without going to another closed itemset
    } else {
      if ( *x<item ) (*cnt)++;
      II->itemflag[*x] = PP->occ_pw[*x] < PP->th? 3: 0; // mark item by freq/infreq
      if ( PP->occ_w[*x] > w ){
        *fmax = *x;
        w = PP->occ_w[*x];
      }
    }
  }
  if ( full && (PP->problem & LCM_POSI_EQUISUPP) && m<item ) // m<item always holds in frequent itemset mining
       LCM_reduce_occ_by_posi_equisupp (PP, occ, item, full);
  return (m);
}

/***************************************************************/
/* iteration of LCM ver. 5 */
/* INPUT: item:tail of the current solution, t_new,buf:head of the list of 
 ID and buffer memory of new transactions */
/*************************************************************************/
void LCM (PROBLEM *PP, int item, QUEUE *occ, WEIGHT frq, WEIGHT pfrq){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  int bnum = TT->buf.num, bblock = TT->buf.block_num;
  int wnum = TT->wbuf.num, wblock = TT->wbuf.block_num;
  VEC_ID new_t = TT->new_t;
  QUEUE_INT cnt, f, *x, m, e, imax = PP->clms? item: TT->T.clms;
  QUEUE_ID js = PP->itemcand.s, qt = II->add.t, i;
  WEIGHT rposi=0.0;

//TRSACT_print (TT, occ, NULL);
//printf ("itemset: %f ::::", II->frq); QUEUE_print__ ( &II->itemset);
//QUEUE_print__ ( occ );
//printf ("itemset: %f ::::", II->frq); QUEUE_perm_print ( &II->itemset, II->perm);
//printf ("add:"); QUEUE_perm_print ( &II->add, II->perm);
//for (i=0 ; i<II->imax ; i++ ) printf ("%d(%d) ", II->perm[i], II->itemflag[i]); printf ("\n");

  II->iters++;
  PP->itemcand.s = PP->itemcand.t;
//  if ( II->flag&ITEMSET_POSI_RATIO && pfrq!=0 ) II->frq /= (pfrq+pfrq-II->frq);
  if ( II->flag&ITEMSET_POSI_RATIO && pfrq!=0 ) rposi = pfrq / (pfrq+pfrq-II->frq);
  TRSACT_delivery (TT, &TT->jump, PP->occ_w, PP->occ_pw, occ, imax);
    // if the itemset is empty, set frq to the original #trsactions, and compute item_frq's
  if ( II->itemset.t == 0 ){
    if ( TT->total_w_org != 0.0 )
        FLOOP (i, 0, TT->T.clms) II->item_frq[i] = PP->occ_w[i]/TT->total_w_org;
  }

  II->frq = frq; II->pfrq = pfrq;
  m = LCM_maximality_check (PP, occ, item, &f, &cnt);
// printf ("add: "); QUEUE_print__ ( &II->add);
  if ( !(PP->problem & PROBLEM_FREQSET) && m<TT->T.clms ){  // ppc check
    MQUE_FLOOP (TT->jump, x) TT->OQ[*x].end = 0;
    goto END;
  }
  if ( !(PP->problem&PROBLEM_MAXIMAL) || f>=TT->T.clms || PP->occ_w[f]<II->frq_lb ){
    if ( !(II->flag & ITEMSET_POSI_RATIO) || (rposi<=II->rposi_ub && rposi>=II->rposi_lb) ){
      II->prob = 1.0;
      MQUE_FLOOP (II->itemset, x) II->prob *= II->item_frq[*x];
      MQUE_FLOOP (II->add, x) II->prob *= II->item_frq[*x];
      ITEMSET_check_all_rule (II, PP->occ_w, occ, &TT->jump, TT->total_pw_org, 0);      //    if (ERROR_MES) return;
    }
  }
    // select freqeut (and addible) items with smaller indices
  MQUE_FLOOP (TT->jump, x){
    TT->OQ[*x].end = 0;  // in the case of freqset mining, automatically done by rightmost sweep;
    if ( *x<item && II->itemflag[*x] == 0 ){
      ARY_INS (PP->itemcand, *x);
      PP->occ_w2[*x] = PP->occ_w[*x];
      if ( TT->flag & TRSACT_NEGATIVE ) PP->occ_pw2[*x] = PP->occ_pw[*x];
    }
  }
  
  if ( QUEUE_LENGTH_(PP->itemcand)==0 || II->itemset.t >= II->ub ) goto END;
  qsort_QUEUE_INT (PP->itemcand.v+PP->itemcand.s, PP->itemcand.t-PP->itemcand.s, -1);
//QUEUE_print__ (&PP->itemcand);
  qsort_QUEUE_INT (II->add.v+qt, II->add.t-qt, -1);

// database reduction
  if ( cnt>2 && (II->flag & ITEMSET_TRSACT_ID)==0 && II->itemset.t >0){
    TRSACT_find_same (TT, occ, item);
    TRSACT_merge_trsact (TT, &TT->OQ[TT->T.clms], item);
    TRSACT_reduce_occ (TT, occ);
  }
// occurrence deliver
  TRSACT_deliv (TT, occ, item);

// loop for recursive calls
  cnt = QUEUE_LENGTH_ (PP->itemcand); f=0;   // for showing progress
  while ( QUEUE_LENGTH_ (PP->itemcand) > 0 ){
    e = QUEUE_ext_tail_ (&PP->itemcand);
    if ( PP->occ_pw2[e] >= MAX(II->frq_lb, II->posi_lb) ){  // if the item is frequent
      LCM_add_item (PP, &II->itemset, e);
      LCM (PP, e, &TT->OQ[e], PP->occ_w2[e], PP->occ_pw2[e]); // recursive call
if ( ERROR_MES ) return;
      LCM_del_item (PP, &II->itemset);
    }
    TT->OQ[e].end = TT->OQ[e].t = 0;   // clear the occurrences, for the further delivery
    PP->occ_w[e] = PP->occ_pw[e] = -WEIGHTHUGE;  // unnecessary?
    
    if ( (II->flag & SHOW_PROGRESS) && (II->itemset.t == 0 ) ){
      f++; print_err ("%d/%d (%lld iterations)\n", f, cnt, II->iters);
    }
  }

  TT->new_t = new_t;
  TT->buf.num = bnum, TT->buf.block_num = bblock;
  TT->wbuf.num = wnum, TT->wbuf.block_num = wblock;

  END:;
  while ( II->add.t > qt ) LCM_del_item (PP, &II->add);
  PP->itemcand.t = PP->itemcand.s;
  PP->itemcand.s = js;
}

/*************************************************************************/
/* initialization of LCM main routine */
/*************************************************************************/
void LCM_init (PROBLEM *PP){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  SGRAPH *SG = &PP->SG;
  PERM *sperm = NULL, *tmp=NULL;
  QUEUE_INT i;

  II->X = TT;
  II->flag |= ITEMSET_ITEMFRQ + ITEMSET_ADD;
  PP->clms = ((PP->problem&PROBLEM_FREQSET)&&(II->flag&ITEMSET_RULE)==0);
  PROBLEM_alloc (PP, TT->T.clms, TT->T.t, 0, TT->perm, PROBLEM_ITEMCAND +(PP->sgraph_fname?PROBLEM_ITEMARY:0) +((TT->flag&TRSACT_NEGATIVE)?PROBLEM_OCC_PW: PROBLEM_OCC_W) +((PP->problem&PROBLEM_FREQSET)?0:PROBLEM_OCC_W2));
  PP->th = (II->flag&ITEMSET_RULE)? ((II->flag&ITEMSET_RULE_INFRQ)? -WEIGHTHUGE: II->frq_lb * II->ratio_lb ): II->frq_lb;  // threshold for database reduction
  if ( TT->flag&TRSACT_SHRINK ) PP->oo = QUEUE_dup_ (&TT->OQ[TT->T.clms]);  // preserve occ
  else { QUEUE_alloc (&PP->oo, TT->T.t); ARY_INIT_PERM(PP->oo.v, TT->T.t); PP->oo.t = TT->T.t; }
  TT->perm = NULL;
  TT->OQ[TT->T.clms].t = 0;
  print_mes (PP->TT.flag, "separated at %d\n", PP->TT.sep);
  if ( !(TT->sc) ) calloc2 (TT->sc, TT->T.clms+2, "LCM_init: item_flag", return);
  free2 (II->itemflag); II->itemflag = TT->sc;  // II->itemflag and TT->sc shares the same memory
  II->frq = TT->total_w_org; II->pfrq = TT->total_pw_org;
  
  if ( PP->sgraph_fname ){
    if ( SG->edge.t < TT->T.clms )
        print_mes (PP->problem, "#nodes in constraint graph is smaller than #items\n");
    if ( TT->perm ){
      malloc2 (sperm, SG->edge.t, "LCM_init: sperm", EXIT);
      ARY_INIT_PERM (sperm, SG->edge.t);
      FLOOP (i, 0, MIN(TT->T.t, SG->edge.t)) sperm[i] = TT->perm[i];
      ARY_INV_PERM (tmp, sperm, SG->edge.t, "LCM_init:INV_PERM", {free(sperm);EXIT;});
      SGRAPH_replace_index (SG, sperm, tmp);
      mfree (tmp, sperm);
      SG->perm = NULL;
    }
    
    SG->edge.flag |= LOAD_INCSORT +LOAD_RM_DUP;
    SETFAMILY_sort (&SG->edge);
  }
  II->total_weight = TT->total_w;
}

/*************************************************************************/
/* main of LCM ver. 5 */
/*************************************************************************/
int LCM_main (int argc, char *argv[]){
  PROBLEM PP;
  ITEMSET *II = &PP.II;
  TRSACT *TT = &PP.TT;
  SGRAPH *SG = &PP.SG;
  
  PROBLEM_init (&PP);
  LCM_read_param (argc, argv, &PP);
if ( ERROR_MES ) return (1);
  TT->flag |= LOAD_PERM +TRSACT_FRQSORT +LOAD_DECSORT +LOAD_RM_DUP +TRSACT_MAKE_NEW +TRSACT_DELIV_SC +TRSACT_ALLOC_OCC + ((II->flag & ITEMSET_TRSACT_ID)?0: TRSACT_SHRINK) ;
  if ( II->flag&ITEMSET_RULE ) TT->w_lb = -WEIGHTHUGE; else TT->w_lb = II->frq_lb;
  SG->flag =  LOAD_EDGE;
  PROBLEM_init2 (&PP, PROBLEM_PRINT_SHRINK + PROBLEM_PRINT_FRQ);
  if ( !ERROR_MES ){
    LCM_init (&PP);
    if ( !ERROR_MES ) LCM (&PP, TT->T.clms, &PP.oo, TT->total_w_org, TT->total_pw_org);
    ITEMSET_last_output (II);
  }

  TT->sc = NULL;
  PROBLEM_end (&PP);
  return (ERROR_MES?1:0);
}

/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (LCM_main (argc, argv));
}
#endif
/*******************************************************************************/

#endif


