/*  QUEUE based Transaction library, including database reduction.
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

#ifndef _trsact_c_
#define _trsact_c_

// #define WEIGHT_DOUBLE

#include"trsact.h"
#include"base.c"
#include"vec.c"

/***********************************/
/*   print transactions            */
/***********************************/
void TRSACT_print (TRSACT *T, QUEUE *occ, PERM *p){
  VEC_ID i, t;
  QUEUE_ID j;
  QUEUE_INT e;
  FLOOP (i, 0, occ? occ->t: T->T.t){
    t = occ? *((QUEUE_INT *)(&(((char *)(occ->v))[i*T->occ_unit]))): i;
    if ( occ ) printf (QUEUE_INTF "::: ", t);
    ARY_FLOOP (T->T.v[t], j, e){
      printf (QUEUE_INTF, p? p[e]: e);
      if ( T->T.w ) printf ("(" WEIGHTF ")", T->T.w[t][j]);
      printf (",");
    }
    if ( T->w ) printf (" :" WEIGHTF " ", T->w[t]);
    printf (" (" QUEUE_INTF ")\n", T->T.v[t].end);
  }
}

/*
void TRSACT_prop_print (TRSACT *T){
  print_err ("trsact: %s", P->trsact_fname);
  if ( P->trsact2_fname2 ) print_err (" ,2nd-trsact2 %s (from ID %d)", P->trsact_fname2, P->TT.end1);
  print_err (" ,#transactions %d ,#items %d ,size %zd", P->TT.rows_org, P->TT.clms_org, P->TT.eles_org);
  print_err (" extracted database: #transactions %d ,#items %d ,size %zd", P->TT.T.t, P->TT.T.clms, P->TT.T.eles);
  if ( P->trsact_wfname ) print_err (" ,weightfile %s", P->trsact_wfname);
  if ( P->trsact_wfname2 ) print_err (" ,2nd-weightfile %s", P->trsact_wfname2);
  if ( P->trsact_pfname ) print_err (" ,item-order-file %s", P->trsact_pfname);
  print_err ("\n");
}
*/

/* initialization of structure TRSACT */
void TRSACT_init (TRSACT *T){
  T->type = TYPE_TRSACT;
  T->flag = 0;
  T->T = INIT_SETFAMILY;
  T->clms_org = T->clm_max = T->clms_end = T->non_empty_clms = 0;
  T->rows_org = T->row_max = T->end1 = T->sep = 0;
  T->perm = NULL;
  T->trperm = NULL;
  T->w = T->pw = NULL;
  
  T->clm_lb = 0;
  T->clm_ub = VEC_ID_END;
  T->row_lb = 0;
  T->row_ub = QUEUE_IDHUGE;
  T->w_lb = -WEIGHTHUGE; T->w_ub = WEIGHTHUGE;
  
  T->eles_org = 0;
  T->total_w = T->total_pw = T->total_w_org = T->total_pw_org =0; 

  T->jump = INIT_QUEUE;
  T->str_num = 0;
  T->head = T->strID = NULL;

  T->th = 1;
  T->mark = NULL;
  T->shift = NULL;
  T->occ_unit = sizeof(QUEUE_INT);
  T->OQ = NULL;
  T->sc = NULL;
  
  T->new_t = 0;
  T->buf = INIT_BASE;
  T->wbuf = INIT_BASE;
}

/**************************************************************/
void TRSACT_end (TRSACT *T){
  if ( T->OQ ){ free2 (T->OQ->v ); free2 (T->OQ[T->T.clms].v); }
  free2 (T->T.w);
  SETFAMILY_end (&T->T);
  if ( T->w != T->pw ) free2 (T->pw);
  mfree (T->w, T->perm, T->trperm);
  mfree (T->mark, T->shift, T->sc, T->OQ, T->head, T->strID);
  QUEUE_end (&T->jump);
  BASE_end (&T->buf);
  BASE_end (&T->wbuf);
  TRSACT_init (T);
}

/*****************************************/
/* scan file "fp" with weight file wfp and count #items, #transactions in the file. */
/*   count weight only if wfp!=NULL                                      */
/* T->rows_org, clms_org, eles_org := #items, #transactions, #all items  */
/*   ignore the transactions of size not in range T->clm_lb - clm_ub     */ 
/* T->total_w, total_pw := sum of (positive) weights of transactions     */
/* C->clmt[i],C->cw[i] := the number/(sum of weights) of transactions including i  */
/****************************************/
void TRSACT_file_count (TRSACT *T, FILE_COUNT *C, FILE2 *fp, char *wf){
  QUEUE_INT i, item, kk=0, k, jump_end=0;
  WEIGHT w, s;
  VEC_ID *jump=NULL;
  FILE2 wfp;
  
  if ( wf ){
    FILE2_open (wfp, wf, "r", "TRSACT_file_count:weight file", goto ERR);
    ARY_SCAN (kk, WEIGHT, wfp, 1);
    kk += T->rows_org;
    realloc2 (C->rw, kk+1, "TRSACT_file_count: C->rw", goto ERR);
    FILE2_reset (&wfp);
    ARY_READ (C->rw, double, kk, wfp);
    ARY_MIN (w, i, C->rw, 0, kk);
    if ( w<0 ) T->flag |= TRSACT_NEGATIVE;
    FILE2_close (&wfp);
  }
  do {
    s=0;
    k=0;
    w = wf? (T->rows_org<kk? C->rw[T->rows_org]: TRSACT_DEFAULT_WEIGHT): 1;
    do {
      item = (QUEUE_INT)FILE2_read_int (fp);
      if ( (FILE_err&4)==0 ){
        ENMAX (T->clms_org, item+1);  // update #items
        reallocx (jump, jump_end, k, 0, "TRSACT_file_count: jump", goto ERR);
        jump[k] = item;
        k++;
        s += wf? (item<kk? MAX(C->rw[item],0): TRSACT_DEFAULT_WEIGHT): 1;

           // count/weight-sum for the transpose mode
        reallocx (C->clmt, C->clm_end, item, 0, "TRSACT_file_count:clmt",goto ERR);
        C->clmt[item]++;
        if ( !(T->flag&LOAD_TPOSE) ){
          reallocx (C->cw, C->cw_end, item, 0, "TRSACT_file_count: cw", goto ERR);
          C->cw[item] += MAX(w,0);    // sum up positive weights
        }
      }
    } while ( (FILE_err&3)==0);

       // count/weight-sum for the transpose mode
    reallocx (C->rowt, C->row_end, T->rows_org, 0, "TRSACT_file_count:rowt", goto ERR);
    C->rowt[T->rows_org] = k;
    if ( T->flag&LOAD_TPOSE ){
      reallocx (C->cw, C->cw_end, T->rows_org, 0, "TRSACT_file_count: cw", goto ERR);
      C->cw[T->rows_org] = s;    // sum up positive weights
    }
    if ( k==0 && FILE_err&2 ) break;
    T->rows_org++;  // increase #transaction
    
    if ( !wf ) s = k;   // un-weighted case; weighted sum is #included-items
    if ( k==0 ){
       T->str_num++;  // increase #streams if empty transaction is read
    } else {
      T->eles_org += k;
      if ( (!(T->flag&LOAD_TPOSE) && !RANGE (T->row_lb, k, T->row_ub))
          || ((T->flag&LOAD_TPOSE) && (!RANGE(T->w_lb, s, T->w_ub) || !RANGE (T->clm_lb, k, T->clm_ub)) ) ) FLOOP (i, 0, k) C->clmt[jump[i]]--; 
    }
  } while ( (FILE_err&2)==0);
  free2 (jump);
    // swap the variables in transpose mode
  if ( C->rw == NULL ){ T->total_w_org = T->total_pw_org = T->rows_org; return; } 
  C->clm_btm = MIN(kk, T->rows_org);
  reallocx (C->rw, kk, T->rows_org, TRSACT_DEFAULT_WEIGHT, "TRSACT_file_count: rw", goto ERR);
  FLOOP (k, 0, T->rows_org){
    T->total_w_org += C->rw[k];
    T->total_pw_org += MAX(C->rw[k],0);
  }
  return;
  ERR:;
  mfree (C->rw, C->cw, C->clmt, C->rowt, jump);
  EXIT;
}

/* allocate memory, set permutation, and free C.clmt,rowt,rw,cw */
int TRSACT_alloc (TRSACT *T, char *pfname, FILE_COUNT *C){
  VEC_ID t, tt=0, ttt=T->clms_org, ttt_max = ttt, h, flag, org;
  FILE_COUNT_INT *ct;
  size_t s=0;
  PERM *q, *p=NULL;
  char *buf;

    // swap variables in the case of transpose
  if ( T->flag & LOAD_TPOSE ){
    common_QUEUE_INT = T->clms_org; T->clms_org = (QUEUE_INT)T->rows_org; T->rows_org = (VEC_ID)common_QUEUE_INT;
    SWAP_PNT (C->clmt, C->rowt);
  }

  if ( T->flag&TRSACT_SHRINK ) T->flag |= LOAD_DBLBUF;
    // count valid columns/elements

  if ( pfname && !(T->flag&TRSACT_WRITE_PERM) ){
    ARY_LOAD (p, QUEUE_INT, ttt, pfname, 1, "TRSACT_load: item order file", EXIT0);
    ARY_MAX (ttt_max, tt, p, 0, ttt);
//    ENMAX (T->clms_org, ttt_max+1);
    T->T.clms = ttt_max+1;
  } else {
    if ( T->flag&LOAD_PERM ){
      if ( T->flag&TRSACT_FRQSORT )
          p = qsort_perm_WEIGHT (C->cw, T->clms_org, (T->flag&LOAD_INCSORT)?1:-1);
      else p = qsort_perm_FILE_COUNT_INT (C->clmt, T->clms_org, (T->flag&LOAD_INCSORT)?1:-1);
    }
    if ( pfname ) ARY_WRITE (pfname, p, T->clms_org, PERMF " ", "TRSACT_alloc: item-order output", EXIT0);
  }
  T->clms_end = MAX (T->clms_org, T->T.clms);
  
  malloc2 (C->cperm, T->clms_org+1, "TRSACT_alloc: cperm", EXIT0);
  ARY_FILL (C->cperm, 0, T->clms_org, T->clms_org+1);
  FLOOP (t, 0, ttt){
    tt = p? p[t]: t;
    if ( tt >= T->clms_org ) continue;
    if ( RANGE(T->w_lb, C->cw[tt], T->w_ub) && RANGE (T->clm_lb, C->clmt[tt], T->clm_ub)){
      s += C->clmt[tt];
      C->cperm[tt] = (pfname && !(T->flag&TRSACT_WRITE_PERM))? t: T->T.clms++;
      T->non_empty_clms++;
    } else C->cperm[tt] = T->clms_end+1;
  }
  free2 (p);
  
    // count valid rows/elements
  if ( T->flag&(LOAD_SIZSORT+LOAD_WSORT) ){
    if ( T->flag&LOAD_WSORT && C->rw )
        p = qsort_perm_WEIGHT (C->rw, T->rows_org, (T->flag&LOAD_DECROWSORT)?-1:1);
    else p = qsort_perm_FILE_COUNT_INT (C->rowt, T->rows_org, (T->flag&LOAD_DECROWSORT)?-1:1);
  }
  malloc2 (C->rperm, T->rows_org, "TRSACT_alloc: rperm", EXIT0);
  FLOOP (t, 0, T->rows_org){  // compute #elements according to rowt, and set rperm
    tt = p? p[t]: t;
    if ( RANGE (T->row_lb, C->rowt[tt], T->row_ub) ){
      C->rperm[tt] = T->T.t++;
      T->T.eles += C->rowt[t];
    } else C->rperm[tt] = T->rows_org+1;
  }

  free2 (p); free2 (C->cw);
  flag = (T->T.eles > s && !(T->flag & LOAD_TPOSE) );
  if ( flag ) T->T.eles = s;

  T->T.end = T->T.t * ((T->flag&LOAD_DBLBUF)? 2: 1)+1;
  malloc2 (T->w, T->T.end, "TRSACT_alloc: T->w", EXIT0);
  if ( TRSACT_NEGATIVE ) malloc2 (T->pw, T->T.end, "TRSACT_alloc: T->pw", EXIT0);
  else T->pw = NULL;
  malloc2 (T->trperm, T->T.t, "TRSACT_alloc: T->trperm", EXIT0);
  malloc2 (T->T.v, T->T.end, "TRSACT_alloc: T->T.v", EXIT0);
  malloc2 (buf, (T->T.eles+T->T.end+1)*T->T.unit, "TRSACT_alloc: T->T.buf", EXIT0);
  T->T.buf = (QUEUE_INT *)buf;
  calloc2 (T->perm, T->T.clms+1, "TRSACT_alloc: T->perm", EXIT0);
  QUEUE_alloc (&T->jump, T->T.clms+1);
  BASE_alloc (&T->buf, sizeof(QUEUE_INT), MAX((int)T->row_max*4,(int)(T->T.eles+T->T.end+1)/10+100));
  BASE_alloc (&T->wbuf, sizeof(WEIGHT), MAX((int)T->row_max*4, (int)(T->T.eles+T->T.end+1)/10+100));
  if ( T->flag&TRSACT_SHRINK ){
    malloc2 (T->mark, T->T.end, "TRSACT_alloc: mark", EXIT0);
    malloc2 (T->shift, T->T.end, "TRSACT_alloc: shift", EXIT0);
    calloc2 (T->sc, T->T.clms, "TRSACT_alloc: sc", EXIT0);
  }
  if ( T->flag&TRSACT_MULTI_STREAM ){
    malloc2 (T->head, T->str_num+2, "TRSACT_alloc: haed", EXIT0);
    malloc2 (T->strID, (T->flag&LOAD_TPOSE)?T->T.clms:T->T.end, "TRSACT_alloc:ID", EXIT0);
  }
  if ( T->flag&TRSACT_UNION )
      calloc2 (T->T.w, T->T.end, "TRSACT_alloc: T->T.w", EXIT0);

if ( ERROR_MES ) return(0);

    // set variables w.r.t rows
  tt=0; FLOOP (t, 0, T->rows_org){
    if ( C->rperm[t] <= T->rows_org ){
      T->T.v[tt] = INIT_QUEUE;
      T->trperm[tt] = t;
      C->rperm[t] = tt;
      T->w[tt] = C->rw? C->rw[t]: 1;
      if ( T->pw ) T->pw[tt] = MAX (T->w[tt], 0);
      if ( !flag ){
        T->T.v[tt].v = (QUEUE_INT *)buf;
        buf += (C->rowt[t]+1)*T->T.unit;
      }
      tt++;
    }
  }
  free2 (C->rw);
    // make the inverse perm of items
  FLOOP (t, 0, T->clms_org)
      if ( C->cperm[t] <= T->clms_end ) T->perm[C->cperm[t]] = t;

    // set head of each stream, and stream ID of each transaction
  if ( T->flag&TRSACT_MULTI_STREAM ){
    malloc2 (T->head, T->str_num+2, "TRSACT_alloc: haed", EXIT0);
    malloc2 (T->strID, (T->flag&LOAD_TPOSE)?T->T.clms:T->T.end, "TRSACT_alloc:ID", EXIT0);
  }
  org = (T->flag&LOAD_TPOSE)? T->clms_org: T->rows_org;
  q = (T->flag&LOAD_TPOSE)? C->cperm: C->rperm;
  ct = (T->flag&LOAD_TPOSE)? C->clmt: C->rowt;
  h=1; tt=0; FLOOP (t, 0, org){
    if ( q[t] <= org ){
      if ( t == T->end1 && T->sep==0 ) T->sep = tt;
      if ( t == T->sep && T->sep>0 ) T->sep = tt;
      if ( T->strID ) T->strID[tt] = h;
      tt++;
    }
    if ( T->head && ct[t]==0 ) T->head[h++] = tt+1;
  }

  T->new_t = T->T.t;
  free2 (C->rowt); free2 (C->clmt);
  return ( flag );
}


/* load the file to allocated memory according to permutation, and free C.rw, C.cw */
void TRSACT_file_read (TRSACT *T, FILE2 *fp, FILE_COUNT *C, VEC_ID *t, int flag){
  QUEUE_INT item;

  FILE2_reset (fp);
  do {
    if ( flag ) T->T.v[*t].v = *t? T->T.v[*t-1].v + T->T.v[*t-1].t +1: T->T.buf;
    do {
      item = (QUEUE_INT)FILE2_read_int (fp);
      if ( (FILE_err&4)==0 ){
//        printf ("%d %d    %d %d\n", C->rperm[*t], T->rows_org, C->cperm[item], T->clms_org );
        if ( T->flag&LOAD_TPOSE ){
          if ( C->rperm[item]<=T->rows_org && C->cperm[*t]<=T->clms_end )
              ARY_INS (T->T.v[ C->rperm[item] ], C->cperm[*t]);
        } else if ( C->rperm[*t]<=T->rows_org && C->cperm[item]<=T->clms_end )
            ARY_INS (T->T.v[ C->rperm[*t] ], C->cperm[item]);
      }
    } while ( (FILE_err&3)==0);
    (*t)++;
  } while ( (FILE_err&2)==0 );
}

/* sort the transactions and items according to the flag, allocate OQ, and database reduction */
void TRSACT_sort (TRSACT *T, FILE_COUNT *C, int flag){
  VEC_ID t, *p;
  int f;
  PERM pp;
  QUEUE Q;
  QUEUE_ID i;

  FLOOP (t, 0, T->T.t)
      T->T.v[t].v[T->T.v[t].t] = T->T.clms;
  if ( flag )
    flag = (T->flag&(LOAD_SIZSORT+LOAD_WSORT)? ((T->flag&LOAD_DECROWSORT)? -1:1):0) *sizeof(QUEUE);
  if ( flag ){   // sort rows for the case that some columns are not read
    qsort_perm__VECt ((VEC *)T->T.v, T->T.t, C->rperm, flag);
    ARY_INVPERMUTE (T->T.v, C->rperm, Q, T->T.t, "TRSACT_sort: ARY_INVPERMUTE", EXIT);
    ARY_INVPERMUTE_ (T->trperm, C->rperm, pp, T->T.t);
  }
  free2 (C->rperm); free2 (C->cperm);

  if ( T->flag & LOAD_PERM ) flag = 1;
  else flag = (T->flag&LOAD_INCSORT)? 1: ((T->flag&LOAD_DECSORT)? -1: 0);
  if ( flag ) FLOOP (t, 0, T->T.t) qsort_QUEUE_INT (T->T.v[t].v, T->T.v[t].t, flag);
  if ( T->flag & LOAD_RM_DUP ) FLOOP (t, 0, T->T.t) MQUE_RM_DUP (T->T.v[t]);
  ST_MAX (T->row_max, i, T->T.v, t, 0, T->T.t);

  if ( T->flag&(TRSACT_ALLOC_OCC+TRSACT_SHRINK) ){
    calloc2 (p, T->T.clms, "TRSACT_sort: p", EXIT);
    QUEUE_delivery (NULL, p, NULL, T->T.v, NULL, T->T.t, T->T.clms);
    ARY_MAX (T->clm_max, i, p, 0, T->T.clms);
    MQUE_ALLOC (T->OQ, T->T.clms, p, T->occ_unit, 1, EXIT);
    QUEUE_alloc (&T->OQ[T->T.clms], MAX(T->T.t, T->clm_max));
    FLOOP (i, 0, T->T.clms+1) T->OQ[i].end = 0;   // end is illegally set to 0, for the use in "TRSACT_find_same" 
    ARY_INIT_PERM (T->OQ[T->T.clms].v, T->T.t);   // initial occurrence := all transactions
    T->OQ[T->T.clms].t = T->T.t;
    free (p);
  }

    // shrinking database
  if ( T->flag&TRSACT_SHRINK ){
    Q = T->OQ[T->T.clms];
    T->OQ[T->T.clms].t = 0;
    TRSACT_find_same (T, &Q, T->T.clms);
    f = T->flag;  // preserve the flag
    BITRM (T->flag ,TRSACT_MAKE_NEW +TRSACT_UNION +TRSACT_INTSEC);
    TRSACT_merge_trsact (T, &T->OQ[T->T.clms], T->T.clms); // just remove duplicated trsacts
    T->flag = f;  // recover flag
    T->OQ[T->T.clms].t = 0;
    FLOOP (t, 0, T->T.t) if ( T->mark[t]>0 ) ARY_INS(T->OQ[T->T.clms], t);  // make resulted occ
  }
  
//  QUEUE_delivery (T->OQ, NULL, NULL, T->T.v, &T->OQ[T->T.clms], T->T.t, T->T.clms);
}

/*****************************************/
/* load transaction file and its weight  */
/*****************************************/
void TRSACT_load (TRSACT *T, char *fname, char *fname2, char *wfname, char *wfname2, char *pfname){
  FILE2 fp, fp2;
  FILE_COUNT C = INIT_FILE_COUNT;
  VEC_ID t=0;
  int f;
  
  FILE2_open (fp, fname, "r", "input-file open error", EXIT);
  if ( fname2 ) FILE2_open (fp2, fname2, "r", "input-file2 open error", EXIT);
  TRSACT_file_count (T, &C, &fp, wfname);                   if (ERROR_MES) goto END;
  T->end1 = T->rows_org;
  if ( fname2 ) TRSACT_file_count (T, &C, &fp2, wfname2);   if (ERROR_MES) goto END;
  f = TRSACT_alloc (T, pfname, &C);                         if (ERROR_MES) goto END;
  TRSACT_file_read (T, &fp, &C, &t, f);                     if (ERROR_MES) goto END;
  if ( fname2 ) TRSACT_file_read (T, &fp2, &C, &t, f);      if (ERROR_MES) goto END;
  TRSACT_sort (T, &C, f);
  
  END:;
  FILE2_close (&fp);
  if (ERROR_MES) TRSACT_end (T);
  return;
}

/* iteration of delivery; operate one transaction */
/* use OQ.end to count the number of items */
/* jump will be cleared (t := s) at the beginning */
void TRSACT_delivery_iter (TRSACT *T, QUEUE *jump, WEIGHT *w, WEIGHT *pw, VEC_ID t, QUEUE_INT m){
  WEIGHT *y=0;
  QUEUE_INT *x;
  int f = T->flag&TRSACT_NEGATIVE;

  if ( T->T.w ) y = T->T.w[t];
  MQUE_MLOOP (T->T.v[t], x, m){
    if ( T->OQ[*x].end == 0 ){ ARY_INS (*jump, *x); w[*x] = 0; if ( f ) pw[*x] = 0; }
    T->OQ[*x].end++;
    if ( y ){
      w[*x] += *y; if ( *y>0 && f) pw[*x] += *y;
      y++;
    } else {
      w[*x] += T->w[t]; if ( f ) pw[*x] += T->pw[t];
    }
  }
}

void TRSACT_delivery (TRSACT *T, QUEUE *jump, WEIGHT *w, WEIGHT *pw, QUEUE *occ, QUEUE_INT m){
  VEC_ID i, t;
  char *b = (char *)(occ?occ->v: NULL);
  jump->t = jump->s;
  FLOOP (i, occ?occ->s:0, occ?occ->t:T->T.t){
    t = occ? *((QUEUE_INT *)b): i;
    TRSACT_delivery_iter (T, jump, w, pw, t, m);
    b += T->occ_unit;
  }
}

/* usual delivery (make transpose) with checking sc
   don't touch jump */
/* if (T->flag&TRSACT_DELIV_SC), do not stack to items e with non-zero T->sc[e] */
void TRSACT_deliv (TRSACT *T, QUEUE *occ, QUEUE_INT m){
  VEC_ID i, t;
  QUEUE_INT *x;
  char *b = (char *)(occ?occ->v: NULL);
  if ( T->flag&TRSACT_DELIV_SC ){
    FLOOP (i, occ?occ->s:0, occ?occ->t:T->T.t){
      t = occ? *((QUEUE_INT *)b): i;
      MQUE_MLOOP (T->T.v[t], x, m)
          if ( !T->sc[*x] ) ARY_INS (T->OQ[*x], t);
      b += T->occ_unit;
    }
  } else {
    FLOOP (i, occ?occ->s:0, occ?occ->t:T->T.t){
      t = occ? *((QUEUE_INT *)b): i;
      MQUE_MLOOP (T->T.v[t], x, m) ARY_INS (T->OQ[*x], t);
      b += T->occ_unit;
    }
  }
}

/**************************************************************/
/* Find identical transactions in a subset of transactions, by radix-sort like method */
/* infrequent items (refer LCM_occ) and items larger than item_max are ignored */
/* INPUT: T:transactions, occ:subset of T represented by indices, result:array for output, item_max:largest item not to be ignored */
/* OUTPUT: if transactions i1, i2,..., ik are the same, they have same value in T->mark[i]
 (not all) isolated transaction may have mark 1 */
/* use 0 to end-1 of QQ temporary, and QQ[i].t and QQ[i].s have to be 0. */
/*************************************************************************/
void TRSACT_find_same (TRSACT *T, QUEUE *occ, QUEUE_INT end){
  VEC_ID mark=0, t_end;
  QUEUE *o=occ, *Q = T->T.v, *EQ, *QQ = T->OQ;
  QUEUE_INT *x, *y, e;
  QUEUE_ID ot = occ->t;

    // initialization
  MQUE_FLOOP (*occ, x){ T->mark[*x] = mark; T->shift[*x] = Q[*x].v; }
  T->jump.t = T->jump.s; QQ[T->T.clms].s = 0;

  while (1){
    if ( o->t - o->s == 1 ) T->mark[o->v[--o->t]] = 1;  // no same transactions; mark by 1
    if ( o->t == 0 ) goto END;
          // if previously inserted transactions are in different group, then change their marks with incrementing mark by one
    mark++; for (x=&o->v[o->s] ; x <&o->v[o->t] ; x++) T->mark[*x] = mark;
    t_end = o->t;
    o->s = o->t = 0;

       // insert each t to buckets
    for (x=o->v ; x<o->v+t_end ; x++){
         // get next item in transaction t
      do {
        e = *(T->shift[*x]);
        T->shift[*x]++;
        if ( e >= end ){ e = T->T.clms; break; }
      } while ( T->sc[e] );
      EQ = &QQ[e];
          // if previously inserted transactions are in different group, then change their mark to the transaction ID of top transacion.
      y = &(EQ->v[EQ->s]);
      if ( EQ->s < EQ->t && T->mark[*y] != T->mark[*x] ){
        if ( EQ->t - EQ->s == 1 ) T->mark[EQ->v[--EQ->t]] = 1; // the tail of the queue has no same transaction; mark the tail by 1
        else {
          mark++; for ( ; y< EQ->v + EQ->t ; y++) T->mark[*y] = mark;
          EQ->s = EQ->t;
        }
      } else if ( EQ->t == 0 && e<T->T.clms ) ARY_INS (T->jump, e);
      ARY_INS (*EQ, *x);  // insert t to bucket of e
    }
    END:;
    if ( QUEUE_LENGTH_(T->jump) == 0 ) break;
    o = &QQ[QUEUE_ext_tail_ (&T->jump)];
  }
  
  // same transactions are in queue of item_max
  if ( QQ[T->T.clms].t -QQ[T->T.clms].s == 1 ) T->mark[QQ[T->T.clms].v[--QQ[T->T.clms].t]] = 1;
  if ( occ != &QQ[T->T.clms] ) occ->t = ot;
}


/****************************************************************************/
/*  copy transaction t to tt (only items i s.t. sc[i]==0)                 **/
/* T->w has to be allocated. itemweight will be alocated even if T->w[t] == NULL */
/****************************************************************************/
void TRSACT_copy (TRSACT *T, VEC_ID tt, VEC_ID t, QUEUE_INT end){
  QUEUE_INT *x, *buf;
  WEIGHT *wbuf = NULL, tw = T->w[t], *w = T->T.w? T->T.w[t]: NULL;
  int bnum = T->buf.num, bblock = T->buf.block_num, wflag = (w || (T->flag&TRSACT_UNION));

  buf = (QUEUE_INT *)BASE_get_memory (&T->buf, T->T.v[t].t+1);
if ( ERROR_MES ) return;
  if ( wflag ) T->T.w[tt] = wbuf = (WEIGHT *)BASE_get_memory (&T->wbuf, T->T.v[t].t+1);
if ( ERROR_MES ){ T->buf.num = bnum; T->buf.block_num = bblock; return; }
  T->T.v[tt].v = buf;
  T->w[tt] = T->w[t];
  if ( T->flag&TRSACT_NEGATIVE ) T->pw[tt] = T->pw[t];
  MQUE_MLOOP (T->T.v[t], x, end){
    if ( !T->sc[*x] ){
      *buf = *x; buf++;
      if ( wflag ){ *wbuf = w? *w: tw; wbuf++; }
    }
    if ( w ) w++;
  }
  T->T.v[tt].t = (VEC_ID)(buf - T->T.v[tt].v);
  *buf = T->T.clms;
  T->buf.num = (int)(buf - ((QUEUE_INT *)T->buf.base[T->buf.block_num]) + 1);
  if ( wflag ) T->wbuf.num = (int)(wbuf - ((WEIGHT *)T->wbuf.base[T->wbuf.block_num]) + 1);
}

/****************************************************************************/
/*  intersection of transaction t and tt (only items i s.t. sc[i]==0)     **/
/*  shift is the array of pointers indicates the start of each transaction **/
/****************************************************************************/
void TRSACT_suffix_and (TRSACT *T, VEC_ID tt, VEC_ID t){
  QUEUE_INT *x=T->shift[tt], *y=T->shift[t], *xx=T->shift[tt];
  while ( *x < T->T.clms && *y < T->T.clms ){
    if ( *x > *y ) y++;
    else {
      if ( *x == *y ){
        if ( !T->sc[*x] ){ *xx = *x; xx++; }
        y++;
      }
      x++;
    }
  }
  T->T.v[tt].t = (VEC_ID)(xx - T->T.v[tt].v);
  *xx = T->T.clms;
  T->buf.num = (int)(xx - ((QUEUE_INT *)T->buf.base[T->buf.block_num]) + 1);
}


/***************************************************************************/
/*  take union of transaction t to tt (only items i s.t. pw[i]>=th)        */
/* CAUSION: t has to be placed at the last of trsact_buf2.                 */
/*   if the size of t inclreases, the following memory will be overwrited  */
/* if memory (T->buf, T->wbuf) is short, do nothing and return 1           */
/* T->T.w[t] can be NULL, but T->T.w[x] can not                            */
/***************************************************************************/
void TRSACT_itemweight_union (TRSACT *T, VEC_ID tt, VEC_ID t){
  int bnum = T->buf.num, bblock = T->buf.block_num;
  QUEUE_ID siz = T->T.v[tt].t +T->T.v[t].t;
  QUEUE_INT *xx_end = T->T.v[tt].v + siz, *xx = xx_end;
  QUEUE_INT *x = T->T.v[tt].v + T->T.v[tt].t-1, *y = T->T.v[t].v + T->T.v[t].t-1;
  WEIGHT *ww = T->T.w[tt] +siz, *wx = T->T.w[tt] +T->T.v[tt].t-1, *wy = T->T.w[t] +T->T.v[t].t-1;
  WEIGHT tw = T->w[t];
  int flag=0, wf = (T->T.w[t]!=NULL);

    // if sufficiently large memory can not be taken from the current memory block, use the next block
  if ( xx_end >= (QUEUE_INT *)T->buf.base[T->buf.block_num] +T->buf.block_siz ){
    xx_end = xx = ((QUEUE_INT*)BASE_get_memory (&T->buf, T->buf.block_siz)) +siz;
if (ERROR_MES) return;
    ww = ((WEIGHT *)BASE_get_memory (&T->wbuf, T->wbuf.block_siz)) +siz;
if ( ERROR_MES ){ T->buf.num = bnum; T->buf.block_num = bblock; return; }
    flag =1;
  }
  if ( ERROR_MES ) return;

    // take union and store it in the allocated memory
  while ( x >= T->T.v[tt].v && y >= T->T.v[t].v ){
    if ( *x > *y ){
      if ( !T->sc[*x] ){ *xx = *x; *ww = *wx; xx--; ww--; }
      x--; wx--;
      if ( x < T->T.v[tt].v ){ 
        while ( y >= T->T.v[t].v ){
          if ( !T->sc[*y] ){ *xx = *y; *ww = wf? *wy: tw;  xx--; ww--; }
          y--; wy--;
        }
      }
    } else {
      if ( !T->sc[*y] ){
        *ww = wf? *wy: tw; *xx = *y;
        if ( *x == *y ){ *ww += *wx; x--; wx--; }
        xx--; ww--;
      }
      y--; wy--;
      if ( y < T->T.v[t].v ){
        while ( x >= T->T.v[tt].v ){
          if ( !T->sc[*x] ){ *xx = *x; *ww = *wx; xx--; ww--; }
          x--; wx--;
        }
      }
    }
  }
  T->T.v[tt].t = (VEC_ID)(xx_end -xx);
  
    // if [tt].v will overflow, set [tt].v to the top of next memory block
  if ( flag ){
    if ( T->T.v[tt].v + T->T.v[tt].t+1 >= (QUEUE_INT *)T->buf.base[T->buf.block_num-1] +T->buf.block_siz ){
      T->T.v[tt].v = (QUEUE_INT *)T->buf.base[T->buf.block_num];
      T->T.w[tt] = (WEIGHT *)T->wbuf.base[T->wbuf.block_num];
    } else {  // new memory block is allocated, but the transaction fits in the previous block
      T->buf.block_num--;
      T->wbuf.block_num--;
    }
  }
    
    // copy the union to the original position
  for ( x=T->T.v[tt].v,wx=T->T.w[tt] ; xx<xx_end ; ){
    xx++; ww++;
    *x = *xx; *wx = *ww;
    x++; wx++;
  }
  *x = T->T.clms;
  T->wbuf.num = T->buf.num = (int)(x - ((QUEUE_INT *)T->buf.base[T->buf.block_num]) +1);
  return;
}



/*****/
/* merge duplicated transactions in occ according to those having same value in T->mark
   the mark except for the representative will be zero, for each group of the same transactions
   the mark of the representative will be its (new) ID +2 (one is for identical transaction) */
/* T->flag&TRSACT_MAKE_NEW: make new trsact for representative
   T->flag&TRSACT_INTSEC: take suffix intersection of the same trsacts
   T->flag&TRSACT_UNION: take union of the same trsacts */
/* o will be cleard after the execution */
void TRSACT_merge_trsact (TRSACT *T, QUEUE *o, QUEUE_INT end){
  VEC_ID mark = 0, tt=0;
  QUEUE_INT *x;

  MQUE_FLOOP (*o, x){
    if ( mark == T->mark[*x] ){
      T->mark[*x] = 0;   // mark of unified (deleted) transaction
      T->w[tt] += T->w[*x]; if ( T->pw ) T->pw[tt] += T->pw[*x];
      if ( T->flag & TRSACT_INTSEC ){
        TRSACT_suffix_and (T, tt, *x);
        T->buf.num = (int)(T->T.v[tt].v - (QUEUE_INT *)T->buf.base[T->buf.block_num]  +T->T.v[tt].t +1);
      }
      if ( T->flag & TRSACT_UNION ){
        TRSACT_itemweight_union (T, tt, *x);
        if ( ERROR_MES ) T->mark[*x] = *x+2; // do not merge if not enough memory
      }
    }
    if ( mark != T->mark[*x] && T->mark[*x] > 1 ){  // *x is not the same to the previous, or memory short 
      mark = T->mark[*x];
      if ( T->flag&TRSACT_MAKE_NEW ){
        tt = T->new_t++;
        TRSACT_copy (T, tt, *x, (T->flag&(TRSACT_INTSEC+TRSACT_UNION))? T->T.clms: end);
        if ( ERROR_MES ){ T->new_t--; tt = *x; }
        else for (T->shift[tt]=T->T.v[tt].v ; *(T->shift[tt])<end ; T->shift[tt]++);
      } else tt = *x;
      T->mark[*x] = tt+2;
    }
  }
  o->t = o->s = 0;
}

/* remove the unified transactions from occ (consider T->occ_unit) */
void TRSACT_reduce_occ (TRSACT *T, QUEUE *occ){
  QUEUE_INT *x, *y=occ->v;
  QUEUE_ID i=0;
  if ( T->occ_unit == sizeof(QUEUE_INT) ){
    MQUE_FLOOP (*occ, x){
      if ( T->mark[*x] == 0 ) continue;
      *y = T->mark[*x]>1? T->mark[*x]-2: *x;
      y++; i++;
    }
  } else {
    MQUE_FLOOP_ (*occ, x, T->occ_unit){
      if ( T->mark[*x] == 0 ) continue;
      memcpy (y, x, T->occ_unit);
      *y = T->mark[*x]>1? T->mark[*x]-2: *x;
      y = (QUEUE_INT *)(((char *)y)+T->occ_unit);
      i++;
    }
  }
  occ->t = i;
}

#endif
