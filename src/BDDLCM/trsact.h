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

#ifndef _trsact_h_
#define _trsact_h_

// #define WEIGHT double
// #define WEIGHT_DOUBLE

#include"vec.h"
#include"base.h"

#ifndef WEIGHT
#define WEIGHT int
#ifdef WEIGHT_DOUBLE
#undef WEIGHT_DOUBLE
#endif
#endif

typedef struct {
  unsigned char type;  // mark to identify type of the structure
  SETFAMILY T;   // transaction
  int flag;      // flag
  WEIGHT *w, *pw;  // weight/positive-weight of transactions

  QUEUE_INT clms_org, clm_max, clms_end, non_empty_clms;  // #items in original file, max size of clms, and max of (original item, internal item)
  VEC_ID rows_org, row_max; // #transactions in the original file
  VEC_ID end1, sep; // #trsact in 1st file, the ID of the last permed trsact of 1st file
  size_t eles_org;  // #elements in the original file
  WEIGHT total_w, total_pw, total_w_org, total_pw_org; 
  WEIGHT th;  // threshold for frequency of items
  PERM *perm, *trperm; // original item permutation loaded from permutation file (and inverse)

  // lower/upper bound of #elements in a column/row. colunmn or row of out of range will be ignored
  VEC_ID clm_lb, clm_ub; 
  QUEUE_ID row_lb, row_ub;  
  WEIGHT w_lb, w_ub;

  VEC_ID str_num;  // number of database (itemset stream/string datasets) in T
  VEC_ID *head, *strID;   // the head (beginning) of each stream, stream ID of each transaction
  int occ_unit;
  
    // for finding same transactions
  QUEUE jump, *OQ;   // queue of non-empty buckets, used in find_same_transactions  
  VEC_ID *mark;  // marks for transactions
  QUEUE_INT **shift;  // memory for shift positions of each transaction
  char *sc;   // flag for non-active (in-frequent) items 
  
    // for extra transactions
  VEC_ID new_t;     // the start ID of un-used transactions
  BASE buf;   // buffer for transaction
  BASE wbuf;   // buffer for itemweights
} TRSACT;

#define TRSACT_FRQSORT 65536  // sort transactions in decreasing order
#define TRSACT_ITEMWEIGHT 131072  // initialize itemweight by transaction weights
#define TRSACT_SHRINK 262144  // do not allocate memory for shrink, but do for mining
#define TRSACT_MULTI_STREAM 524288  // separate the datasets at each empty transaction
#define TRSACT_UNION 1048576  // take union of transactions, at the database reduction
#define TRSACT_INTSEC 2097152 // take intersection of transactions, at the database reduction
#define TRSACT_MAKE_NEW 4194304  // make new transaction for each 
#define TRSACT_ALLOC_OCC 8388608  // make new transaction for each 
#define TRSACT_DELIV_SC 16777216  // look T->sc when delivery
#define TRSACT_NEGATIVE 33554432  // flag for whether some transaction weights are negative or not 
//#define TRSACT_INIT_SHRINK 65536  // allocate memory for database reduction
#define TRSACT_WRITE_PERM 67108864  // write item-order to file

#ifndef TRSACT_DEFAULT_WEIGHT
 #define TRSACT_DEFAULT_WEIGHT 0  // default weight of the transaction, for missing weights in weight file
#endif

/*   print transactions */
void TRSACT_print (TRSACT *T, QUEUE *occ, PERM *p);
void TRSACT_prop_print (TRSACT *T);

/**************************************************************/
void TRSACT_init (TRSACT *T);

/**************************************************************/
void TRSACT_end (TRSACT *T);

/*****************************************/
/* scan file "fp" with weight file wfp and count #items, #transactions in the file. */
/*   count weight only if wfp!=NULL                                      */
/* T->rows_org, clms_org, eles_org := #items, #transactions, #all items  */
/*   ignore the transactions of size not in range T->clm_lb - clm_ub     */ 
/* T->total_w, total_pw := sum of (positive) weights of transactions     */
/* C.clmt[i],C.cw[i] := the number/(sum of weights) of transactions including i  */
/****************************************/
void TRSACT_file_count (TRSACT *T, FILE_COUNT *C, FILE2 *fp, char *wf);

/* allocate memory, set permutation, and free C.clmt,rowt,rw,cw */
int TRSACT_alloc (TRSACT *T, char *pfname, FILE_COUNT *C);

/* load the file to allocated memory according to permutation, and free C.rw, C.cw */
void TRSACT_file_read (TRSACT *T, FILE2 *fp, FILE_COUNT *C, VEC_ID *t, int flag);

/*****************************************/
/*     load transaction file to TRSACT   */
void TRSACT_load (TRSACT *T, char *fname, char *fname2, char *wfname, char *wfname2, char *pfname);

/* occurrence deliver (only counting) */
/* WARNING: next cell of the last item of each transaction must be INTHUGE */
/* compute occurrence for items less than max item, in the database induced
 by occ */
/* if jump!=0, all i with non-zero occ[i].t will be inserted to jump */
/* be careful for overflow of jump */
/* if occ==NULL, scan all transactions */
/* flag&1: count only positive weights */
void TRSACT_delivery_iter (TRSACT *T, QUEUE *jump, WEIGHT *w, WEIGHT *pw, VEC_ID t, QUEUE_INT m);
void TRSACT_delivery (TRSACT *T, QUEUE *jump, WEIGHT *w, WEIGHT *pw, QUEUE *occ, QUEUE_INT m);
// QUEUE *TRSACT_alloc_occ (TRSACT *T, QUEUE_INT end);
//QUEUE_ID TRSACT_occ_dup (SETFAMILY *S, QUEUE *OQ, QUEUE *jump, WEIGHT *occ_w, WEIGHT *occ_pw);

/**************************************************************/
/* Find identical transactions in a subset of transactions, by radix-sort like method */
/* infrequent items (refer LCM_occ) and items larger than item_max are ignored */
/* INPUT: T:transactions, occ:subset of T represented by indices, result:array for output, item_max:largest item not to be ignored */
/* OUTPUT: if transactions i1, i2,..., ik are the same, they have same value in T->mark[i]
 (not all) isolated transaction may have mark 1 */
/* use 0 to end-1 of QQ temporary, and QQ[i].t and QQ[i].s have to be 0. */
/*************************************************************************/
void TRSACT_find_same (TRSACT *T, QUEUE *occ, QUEUE_INT end);

/*  copy transaction t to tt (only items i s.t. pw[i]>=th)                 **/
void TRSACT_copy (TRSACT *T, VEC_ID tt, VEC_ID t, QUEUE_INT end);

/*  intersection of transaction t and tt (only items i s.t. pw[i]>=th)     **/
/*  shift is the array of pointers indicates the start of each transaction **/
void TRSACT_suffix_and (TRSACT *T, VEC_ID tt, VEC_ID t);

/*  take union of transaction t to tt (only items i s.t. pw[i]>=th)        */
/* CAUSION: t has to be placed at the last of trsact_buf2.                 */ 
/*   if the size of t inclreases, the following memory will be overwrited  */
/* if memory (T->buf) is short, do nothing and return 1               */
void TRSACT_itemweight_union (TRSACT *T, VEC_ID tt, VEC_ID t);


/*****/
/* remove duplicated transactions from occ, and add the weight of the removed trsacts to the representative one */
/* duplicated trsacts are in occ[item_max]. Clear the queue when return */
/* T->flag&TRSACT_MAKE_NEW: make new trsact for representative
   T->flag&TRSACT_INTSEC: take suffix intersection of the same trsacts
   T->flag&TRSACT_UNION: take union of the same trsacts */
void TRSACT_merge_trsact (TRSACT *T, QUEUE *o, QUEUE_INT end);

/* remove the unified transactions from occ (consider T->occ_unit) */
void TRSACT_reduce_occ (TRSACT *T, QUEUE *occ);

#ifdef _alist_h_

/* occurrence deliver (only counting), for MALIST */
//void TRSACT_MALIST_delivery (TRSACT *T, QUEUE *jump, WEIGHT *w, WEIGHT *pw, MALIST *occ, ALIST_ID l, QUEUE_INT m);
//void TRSACT_MALIST_occ_deliver (TRSACT *TT, MALIST *occ, int l, int item_max);

#endif

#endif
