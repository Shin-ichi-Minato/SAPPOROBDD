/*  itemset search input/output common routines
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

/* routines for itemset mining */

#ifndef _itemset_c_
#define _itemset_c_

#include"itemset.h"
#include"queue.c"
#include"aheap.c"

/* flush the write buffer, available for multi-core mode */
void ITEMSET_flush (ITEMSET *I, FILE2 *fp){
  if ( !(I->flag&ITEMSET_MULTI_OUTPUT) || (fp->buf-fp->buf_org) > FILE2_BUFSIZ/2 ){
    SPIN_LOCK(I->multi_core, I->lock_output);
    FILE2_flush (fp);
    SPIN_UNLOCK(I->multi_core, I->lock_output);
  }
}

/* Output information about ITEMSET structure. flag&1: print frequency constraint */
void ITEMSET_print (ITEMSET *I, int flag){
  if ( I->lb>0 || I->ub<INTHUGE ){
    if ( I->lb > 0 ) print_err ("%d <= ", I->lb);
    print_err ("itemsets ");
    if ( I->ub < INTHUGE ) print_err (" <= %d\n", I->ub);
    print_err ("\n");
  }
  if ( flag&1 ){
    if ( I->frq_lb > -WEIGHTHUGE ) print_err (WEIGHTF" <=", I->frq_lb);
    print_err (" frequency ");
    if ( I->frq_ub < WEIGHTHUGE ) print_err (" <="WEIGHTF, I->frq_ub);
    print_err ("\n");
  }
}

/* ITEMSET initialization */
void ITEMSET_init (ITEMSET *I){
  I->flag = 0;
  I->iters = I->iters2 = I->iters3 = 0;
  I->solutions = I->solutions2 = I->max_solutions = I->outputs = I->outputs2 = 0;
  I->topk.end = 0;
  I->item_max = I->item_max_org = 0;
  I->ub = I->len_ub = I->gap_ub = INTHUGE;
  I->lb = I->len_lb = I->gap_lb = 0;
  I->frq = I->pfrq = I->total_weight = 0;
  I->ratio = I->prob = 0.0;
  I->posi_ub = I->nega_ub = I->frq_ub = WEIGHTHUGE;
  I->posi_lb = I->nega_lb = I->frq_lb = I->setrule_lb = -WEIGHTHUGE;
  I->dir = 0;
  I->target = INTHUGE;
  I->prob_ub = I->ratio_ub = I->rposi_ub = 1;
  I->prob_lb = I->ratio_lb = I->rposi_lb = 0;
  I->itemflag = NULL;
  I->perm = NULL;
  I->item_frq = NULL;
  I->sc = NULL;
  I->X = NULL;
  I->fp = NULL;
  I->topk = INIT_AHEAP;
  I->itemset = I->add = INIT_QUEUE;
  I->set_weight = NULL;
  I->set_occ = NULL;

  I->multi_iters = I->multi_iters2 = I->multi_iters3 = NULL;
  I->multi_outputs = I->multi_outputs2 = NULL;
  I->multi_solutions = I->multi_solutions2 = NULL;
  I->multi_fp = NULL;
  I->multi_core = 0;
}


/* second initialization
   topk.end>0 => initialize heap for topk mining */
/* all pointers will be set to 0, but not for */
/* if topK mining, set topk.end to "K" */
void ITEMSET_init2 (ITEMSET *I, char *fname, PERM *perm, QUEUE_INT item_max, size_t item_max_org){
  LONG i;
  size_t siz = (I->flag&ITEMSET_USE_ORG)?item_max_org+2: item_max+2;
  I->prob = I->ratio = 1.0;
  I->frq = 0;
  I->perm = perm;
  if ( I->topk.end>0 ){
    AHEAP_alloc (&I->topk, I->topk.end);
    FLOOP (i, 0, I->topk.end) AHEAP_chg (&I->topk, (AHEAP_ID)i, -WEIGHTHUGE);
    I->frq_lb = -WEIGHTHUGE;
  } else I->topk.v = NULL;
  QUEUE_alloc (&I->itemset, (QUEUE_ID)siz); I->itemset.end = (QUEUE_ID)siz;
  if ( I->flag&ITEMSET_ADD ) QUEUE_alloc (&I->add, (QUEUE_ID)siz);
  calloc2 (I->sc, siz+2, "ITEMSET_init2: sc", goto ERR);
  if ( I->flag&ITEMSET_SET_RULE ){
    calloc2 (I->set_weight, siz, "ITEMSET_init2: set_weight", goto ERR);
    if ( I->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT) )
        calloc2 (I->set_occ, siz, "ITEMSET_init2: set_weight", goto ERR);
  }
  I->iters = I->iters2 = I->solutions = 0;
  I->item_max = item_max;
  I->item_max_org = (QUEUE_INT)item_max_org;
  if ( fname ){ fopen2 (I->fp, fname, "w", "ITEMSET_init2", goto ERR);}
  else I->fp = 0;
  if ( I->flag&ITEMSET_ITEMFRQ )
    malloc2 (I->item_frq, item_max+2, "ITEMSET_init2: item_frqs", goto ERR);
  if ( I->flag&ITEMSET_RULE ){
    calloc2 (I->itemflag, item_max+2, "ITEMSET_init2: item_flag", goto ERR);
  }
  I->total_weight = 1;

  calloc2 (I->multi_iters, I->multi_core+1, "ITEMSET_init2: multi_iters", goto ERR);
  calloc2 (I->multi_iters2, I->multi_core+1, "ITEMSET_init2: multi_iters2", goto ERR);
  calloc2 (I->multi_iters3, I->multi_core+1, "ITEMSET_init2: multi_iters3", goto ERR);
  calloc2 (I->multi_outputs, I->multi_core+1, "ITEMSET_init2: multi_outputs", goto ERR);
  calloc2 (I->multi_outputs2, I->multi_core+1, "ITEMSET_init2: multi_outputs2", goto ERR);
  calloc2 (I->multi_solutions, I->multi_core+1, "ITEMSET_init2: multi_solutions", goto ERR);
  calloc2 (I->multi_solutions2, I->multi_core+1, "ITEMSET_init2: multi_solutions2", goto ERR);
  calloc2 (I->multi_fp, I->multi_core+1, "ITEMSET_init2: multi_fp", goto ERR);

  FLOOP (i, 0, MAX(I->multi_core,1))
      FILE2_open_ (I->multi_fp[i], I->fp, "ITEMSET_init2: multi_fp[i]", goto ERR);
#ifdef MULTI_CORE
  if ( I->multi_core > 0 ){
    pthread_spin_init (&I->lock_counter, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init (&I->lock_sc, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init (&I->lock_output, PTHREAD_PROCESS_PRIVATE);
  }
#endif
  return;
  ERR:;
  ITEMSET_end (I);
  EXIT;
}

/* sum the counters computed by each thread */
void ITEMSET_merge_counters (ITEMSET *I){
  int i;
  FLOOP (i, 0, MAX(I->multi_core,1)){
    I->iters += I->multi_iters[i];
    I->iters2 += I->multi_iters2[i];
    I->iters3 += I->multi_iters3[i];
    I->outputs += I->multi_outputs[i];
    I->outputs2 += I->multi_outputs2[i];
    I->solutions += I->multi_solutions[i];
    I->solutions2 += I->multi_solutions2[i];
    FILE2_flush ( &I->multi_fp[i]);
  }
}

/*******************************************************************/
/* termination of ITEMSET */
/*******************************************************************/
void ITEMSET_end (ITEMSET *I){
  int i;
  QUEUE_end (&I->itemset);
  QUEUE_end (&I->add);
  AHEAP_end (&I->topk);
  fclose2 ( I->fp);
  mfree (I->sc, I->item_frq, I->itemflag, I->perm, I->set_weight, I->set_occ);
  
  if ( I->multi_fp )
      FLOOP (i, 0, MAX(I->multi_core,1)) free2 (I->multi_fp[i].buf);
  mfree (I->multi_fp, I->multi_iters, I->multi_iters2, I->multi_iters3);
  mfree (I->multi_outputs, I->multi_outputs2, I->multi_solutions, I->multi_solutions2);
#ifdef MULTI_CORE
  if ( I->multi_core>0 ){
    pthread_spin_destroy(&I->lock_counter);
    pthread_spin_destroy(&I->lock_sc);
    pthread_spin_destroy(&I->lock_output);
  }
#endif
  ITEMSET_init (I);
}

/*******************************************************************/
/* output at the termination of the algorithm */
/* print #of itemsets of size k, for each k */
/*******************************************************************/
void ITEMSET_last_output (ITEMSET *I){
  QUEUE_ID i;
  unsigned long long n=0, nn=0;

  ITEMSET_merge_counters (I);
  if ( I->topk.end > 0 ){
    i = AHEAP_findmin_head (&I->topk);
    fprint_WEIGHT (stdout, AHEAP_H (I->topk, i));
    printf ("\n");
    return;
  }
  FLOOP (i, 0, I->itemset.end+1){
    n += I->sc[i];
    if ( I->sc[i] != 0 ) nn = i;
  }
  if ( !(I->flag&SHOW_MESSAGE) ) return;
  if ( n!=0 ){
    printf ("%llu\n", n);
    FLOOP (i, 0, nn+1) printf ("%llu\n", I->sc[i]);
  }
  print_err ("iters=%lld", I->iters);
  if ( I->flag&ITEMSET_ITERS2 ) print_err (", iters2=%lld", I->iters2);
  print_err ("\n");
}

/* output frequency, coverage */
void ITEMSET_output_frequency (ITEMSET *I, int core_id){
  FILE2 *fp = &I->multi_fp[core_id];
  if ( I->flag&(ITEMSET_FREQ+ITEMSET_PRE_FREQ) ){
    if ( I->flag&ITEMSET_FREQ ) FILE2_putc (fp, ' ');
    FILE2_print_WEIGHT (fp, I->frq, 4, '(');
    FILE2_putc (fp, ')');
    if ( I->flag&ITEMSET_PRE_FREQ ) FILE2_putc (fp, ' ');
  }
  if ( I->flag&ITEMSET_OUTPUT_POSINEGA ){ // output positive sum, negative sum in the occurrence
    FILE2_putc (fp, ' ');
    FILE2_print_WEIGHT (fp, I->pfrq, 4, '(');
    FILE2_print_WEIGHT (fp, I->pfrq-I->frq, 4, ',');
    FILE2_print_WEIGHT (fp, I->pfrq/(2*I->pfrq-I->frq), 4, ',');
    FILE2_putc (fp, ')');
  }
}

#ifdef _trsact_h_
void ITEMSET_output_occ (ITEMSET *I, QUEUE *occ, int core_id){
  QUEUE_ID i;
  QUEUE_INT *x;
  FILE2 *fp = &I->multi_fp[core_id];
  TRSACT *TT = (TRSACT *)(I->X);
  VEC_ID j, ee = TT->rows_org;
  int flag = I->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT);

  i=0; MQUE_FLOOP_ (*occ, x, TT->occ_unit){
    if ( (I->flag&ITEMSET_RM_DUP_TRSACT)==0 || *x != ee ){
      FILE2_print_int (fp, TT->trperm? TT->trperm[*x]: *x,' ');
      if (flag == ITEMSET_MULTI_OCC_PRINT ){
        FLOOP (j, 1, (VEC_ID)(TT->occ_unit/sizeof(QUEUE_INT)))
            FILE2_print_int (fp, *(x+j), ' ');
      } else if ( flag == (ITEMSET_MULTI_OCC_PRINT+ITEMSET_TRSACT_ID) ){
         FILE2_print_int (fp, *(x+1), ' ');
      }
    }
    ee = *x;
    if ( (++i)%256==0 ) ITEMSET_flush (I, fp);
  }
  FILE2_putc (fp, '\n');
}
#endif

/* output an itemset to the output file */
void ITEMSET_output_itemset (ITEMSET *I, QUEUE *occ, int core_id){
  QUEUE_ID i;
  QUEUE_INT e;
  FILE2 *fp = &I->multi_fp[core_id];
  
  I->multi_outputs[core_id]++;
  if ( (I->flag&SHOW_PROGRESS ) && (I->iters%(ITEMSET_INTERVAL) == 0) )
      print_err ("---- %lld solutions in %lld candidates\n", I->solutions, I->outputs);
  if ( I->itemset.t < I->lb || I->itemset.t > I->ub ) return;
  if ( (I->flag&ITEMSET_IGNORE_BOUND)==0 && (I->frq < I->frq_lb || I->frq > I->frq_ub) ) return;
  if ( (I->flag&ITEMSET_IGNORE_BOUND)==0 && (I->pfrq < I->posi_lb || I->pfrq > I->posi_ub || (I->frq - I->pfrq) > I->nega_ub || (I->frq - I->pfrq) < I->nega_lb) ) return;

  I->multi_solutions[core_id]++;
  if ( I->max_solutions>0 && I->solutions > I->max_solutions ){
    ITEMSET_last_output (I);
    ERROR_MES = "reached to maximum number of solutions";
    EXIT;
  }
  if ( I->topk.v ){
    e = AHEAP_findmin_head (&(I->topk));
    if ( I->frq > AHEAP_H (I->topk, e) ){
      AHEAP_chg (&(I->topk), e, I->frq);
      e = AHEAP_findmin_head (&(I->topk));
      I->frq_lb = AHEAP_H (I->topk, e);
    }
  } else if ( I->fp ){
    if ( I->flag&ITEMSET_PRE_FREQ ) ITEMSET_output_frequency (I, core_id);
    if ( (I->flag & ITEMSET_NOT_ITEMSET) == 0 ){
#ifdef _agraph_h_
      if ( I->flag&ITEMSET_OUTPUT_EDGE ){
        ARY_FLOOP (I->itemset, i, e){
          FILE2_print_int (fp, AGRAPH_INC_FROM(*((AGRAPH *)(I->X)),e,I->dir), '(' );
          FILE2_print_int (fp, AGRAPH_INC_TO(*((AGRAPH *)(I->X)),e,I->dir), ',');
          FILE2_putc (fp, ')');
          if ( i<I->itemset.t-1 ) FILE2_putc (fp, ' ');
          if ( (i+1)%256==0 ) ITEMSET_flush (I, fp);
        }
        goto NEXT;
      }
#endif
      ARY_FLOOP (I->itemset, i, e){
        FILE2_print_int (fp,  I->perm? I->perm[e]: e, i==0? 0: ' ');
        if ( (i+1)%256==0 ) ITEMSET_flush (I, fp);
      }
#ifdef _agraph_h_
      NEXT:;
#endif
    }
    if ( !(I->flag&ITEMSET_PRE_FREQ) ) ITEMSET_output_frequency (I, core_id);
    if ( ((I->flag & ITEMSET_NOT_ITEMSET) == 0) || (I->flag&ITEMSET_FREQ) || (I->flag&ITEMSET_PRE_FREQ) ) FILE2_putc (fp, '\n');

#ifdef _trsact_h_
    if (I->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT)) ITEMSET_output_occ (I, occ, core_id);
#endif
  }
  I->sc[I->itemset.t]++;
  ITEMSET_flush (I, fp);
}

/* output itemsets with adding all combination of "add"
   at the first call, i has to be "add->t" */
void ITEMSET_solution_iter (ITEMSET *I, QUEUE *occ, int core_id){
  QUEUE_ID t=I->add.t;
  if ( I->itemset.t > I->ub ) return;
  ITEMSET_output_itemset (I, occ, core_id);
if ( ERROR_MES ) return;
  BLOOP (I->add.t, I->add.t, 0){
    ARY_INS (I->itemset, I->add.v[I->add.t]);
    ITEMSET_solution_iter (I, occ, core_id);
if ( ERROR_MES ) return;
    I->itemset.t--;
  }
  I->add.t = t;
}

void ITEMSET_solution (ITEMSET *I, QUEUE *occ, int core_id){
  QUEUE_ID i;
  LONG s;
  if ( I->itemset.t > I->ub ) return;
  if ( I->flag & ITEMSET_ALL ){
    if ( I->fp || I->topk.v ) ITEMSET_solution_iter (I, occ, core_id);
    else {
      s=1; FLOOP (i, 0, I->add.t+1){
        I->sc[I->itemset.t+i] += s;
        s = s*(I->add.t-i)/(i+1);
      }
    }
  } else {
    FLOOP (i, 0, I->add.t) ARY_INS (I->itemset, I->add.v[i]);
    ITEMSET_output_itemset (I, occ, core_id);
    I->itemset.t -= I->add.t;
  }
}

/*************************************************************************/
/* ourput a rule */
/*************************************************************************/
void ITEMSET_output_rule (ITEMSET *I, QUEUE *occ, double p1, double p2, size_t item, int core_id){
  FILE2 *fp = &I->multi_fp[core_id];
  if ( fp->fp && !(I->topk.v) ){
    FILE2_print_real (fp, p1, 4, '(');
    FILE2_print_real (fp, p2, 4, ',');
    FILE2_putc (fp, ')');
    FILE2_print_int (fp, I->perm[item], ' ');
    FILE2_puts (fp, " <= ");
  }
  if ( I->flag & ITEMSET_RULE ) ITEMSET_output_itemset (I, occ, core_id);
  else ITEMSET_solution (I, occ, core_id);
}
/*************************************************************************/
/* check all rules for a pair of itemset and item */
/*************************************************************************/
void ITEMSET_check_rule (ITEMSET *I, WEIGHT *w, QUEUE *occ, size_t item, int core_id){
  double p = w[item]/I->frq, pp, ff;
//  printf ("[ratio] %f, p=%f, (%f/ %f), %d(%d) <= ", I->ratio_lb, p, w[item], I->frq, I->perm[item], I->itemflag[item]);
  if ( I->itemflag[item]==1 ) return;
  if ( w[item] <= -WEIGHTHUGE ) p = 0;
  pp = p; ff = I->item_frq[item]; 
  if ( I->flag & ITEMSET_RULE_SUPP ){ pp = w[item]; ff *= I->total_weight; }

  if ( I->flag & (ITEMSET_RULE_FRQ+ITEMSET_RULE_INFRQ)){
    if ( (I->flag & ITEMSET_RULE_FRQ) && p < I->ratio_lb ) return;
    if ( (I->flag & ITEMSET_RULE_INFRQ) && p > I->ratio_ub ) return;
    ITEMSET_output_rule (I, occ, pp, ff, item, core_id);
  } else if ( I->flag & (ITEMSET_RULE_RFRQ+ITEMSET_RULE_RINFRQ) ){
    if ( (I->flag & ITEMSET_RULE_RFRQ) && (1-p) > I->ratio_lb * (1-I->item_frq[item]) ) return;
    if ( (I->flag & ITEMSET_RULE_RINFRQ) && p > I->ratio_ub * I->item_frq[item] ) return;
    ITEMSET_output_rule (I, occ, pp, ff, item, core_id);
  }
}

/*************************************************************************/
/* check all rules for an itemset and all items */
/*************************************************************************/
void ITEMSET_check_all_rule (ITEMSET *I, WEIGHT *w, QUEUE *occ, QUEUE *jump, WEIGHT total, int core_id){
  QUEUE_ID i, t;
  QUEUE_INT e, f=0, *x;
  WEIGHT d = I->frq/total;

    // checking out of range for itemset size and (posi/nega) frequency
  if ( I->itemset.t+I->add.t < I->lb || I->itemset.t>I->ub || (!(I->flag&ITEMSET_ALL) && I->itemset.t+I->add.t>I->ub)) return;
  if ( !(I->flag&ITEMSET_IGNORE_BOUND) && (I->frq < I->frq_lb || I->frq > I->frq_ub) ) return;
  if ( !(I->flag&ITEMSET_IGNORE_BOUND) && (I->pfrq < I->posi_lb || I->pfrq > I->posi_ub || (I->frq - I->pfrq) > I->nega_ub || (I->frq - I->pfrq) < I->nega_lb) ) return;

  if ( I->flag&ITEMSET_SET_RULE ){  // itemset->itemset rule for sequence mining
    FLOOP (i, 0, I->itemset.t-1){
      if ( I->frq/I->set_weight[i] >= I->setrule_lb && I->fp ){
        I->sc[i]++;
        if ( I->flag&ITEMSET_PRE_FREQ ) ITEMSET_output_frequency (I, core_id);
        FLOOP (t, 0, I->itemset.t){
          FILE2_print_int (&I->multi_fp[core_id], I->itemset.v[t], t?' ':0);
          if ( t == i ){
            FILE2_putc (&I->multi_fp[core_id], ' ');
            FILE2_putc (&I->multi_fp[core_id], '=');
            FILE2_putc (&I->multi_fp[core_id], '>');
          }
        }
        if ( !(I->flag&ITEMSET_PRE_FREQ) ) ITEMSET_output_frequency ( I, core_id);
        FILE2_putc (&I->multi_fp[core_id], ' ');
        FILE2_print_real (&I->multi_fp[core_id], I->frq/I->set_weight[i], 4, '(');
        FILE2_putc (&I->multi_fp[core_id], ')');
        FILE2_putc (&I->multi_fp[core_id], '\n');
#ifdef _trsact_h_
        if ( I->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT) )
            ITEMSET_output_occ (I, I->set_occ[i], core_id);
#endif
        ITEMSET_flush (I, &I->multi_fp[core_id]);
      }
    }
  }
    // constraint of relational frequency
  if ( ((I->flag&ITEMSET_RFRQ)==0 || d >= I->prob_lb * I->prob ) 
      && ((I->flag&ITEMSET_RINFRQ)==0 || d <= I->prob * I->prob_ub) ){
    if ( I->flag&ITEMSET_RULE ){  //  rule mining routines
      if ( I->itemset.t == 0 ) return;
      if ( I->target < I->item_max ){
        ITEMSET_check_rule (I, w, occ, I->target, core_id);    if (ERROR_MES) return;
      } else {
        if ( I->flag & (ITEMSET_RULE_FRQ + ITEMSET_RULE_RFRQ) ){
          if ( I->add.t>0 ){
//            if ( I->itemflag[I->add.v[0]] ) // for POSI_EQUISUPP (occ_w[e] may not be 100%, in the case)
            f = I->add.v[I->add.t-1]; t = I->add.t; I->add.t--;
            FLOOP (i, 0, t){
              e = I->add.v[i];
              I->add.v[i] = f;
              ITEMSET_check_rule (I, w, occ, e, core_id);    if (ERROR_MES) return;
              I->add.v[i] = e;
            }
            I->add.t++;
          }
          MQUE_FLOOP (*jump, x)
              ITEMSET_check_rule (I, w, occ, *x, core_id);   if (ERROR_MES) return;
        } else {
          if ( I->flag & (ITEMSET_RULE_INFRQ + ITEMSET_RULE_RINFRQ) ){
//          ARY_FLOOP ( *jump, i, e ) I->itemflag[e]--;
            FLOOP (i, 0, I->item_max){
              if ( I->itemflag[i] != 1 ){
                ITEMSET_check_rule (I, w, occ, i, core_id);     if (ERROR_MES) return;
              }
            }
//          ARY_FLOOP ( *jump, i, e ) I->itemflag[e]++;
//        } 
//        ARY_FLOOP ( *jump, i, e ) ITEMSET_check_rule (I, w, occ, e);
          }
        }
      }
    } else {  // usual mining (not rule mining)
      if ( I->fp && (I->flag&(ITEMSET_RFRQ+ITEMSET_RINFRQ))){
        FILE2_print_real (&I->multi_fp[core_id], d, 4, '[');
        FILE2_print_real (&I->multi_fp[core_id], I->prob, 4, ',');
        FILE2_putc (&I->multi_fp[core_id], ']');
      }
      ITEMSET_solution (I, occ, core_id);
    }
  }
}

#endif
