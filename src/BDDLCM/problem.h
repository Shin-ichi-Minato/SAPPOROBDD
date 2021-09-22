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

#ifndef _problem_h_
#define _problem_h_

#include"stdlib2.h"
#include"queue.h"
#include"itemset.h"

#define PROBLEM_FREQSET 1
#define PROBLEM_MAXIMAL 2
#define PROBLEM_CLOSED 4
#define PROBLEM_EX_MAXIMAL 8
#define PROBLEM_EX_CLOSED 16

/*****  parameters for PROBLEM initialization, given to flag  *****/

#define PROBLEM_PRINT_DENSE 4  // print density threshold
#define PROBLEM_PRINT_SHRINK 8  // print properties of shrinked database
#define PROBLEM_PRINT_FRQ   16  // print density threshold
#define PROBLEM_NORMALIZE   32  // print density threshold

#define PROBLEM_ITEMARY 128 // alloc itemary
#define PROBLEM_ITEMJUMP 256 // alloc itemjump
#define PROBLEM_ITEMFLAG 512  // alloc itemflag
#define PROBLEM_ITEMMARK 1024  // alloc itemmark
#define PROBLEM_ITEMCAND 2048 // alloc itemcand
#define PROBLEM_VECARY 4096 // alloc itemary
#define PROBLEM_VECJUMP 8192 // alloc vecjump
#define PROBLEM_VECFLAG 16384  // alloc vecflag
#define PROBLEM_VECMARK 32768  // alloc vecmark
#define PROBLEM_VECCAND 65536 // alloc veccand
//4194304
#define PROBLEM_OCC_T 524288 // alloc occ_t
#define PROBLEM_SHIFT 1048576  // allocate shift
#define PROBLEM_OCC_W 2097152  // weight/positive-weight sum for items
#define PROBLEM_OCC_PW 4194304  // weight/positive-weight sum for items
#define PROBLEM_OCC_W2 8388608  // weight/positive-weight sum for items

#define PROBLEM_OCC1 16 // alloc occ
#define PROBLEM_OCC2 32 // alloc occ and ins all to list 0
#define PROBLEM_OCC3 48 // alloc occ and ins all to list "siz"

typedef struct {
  clock_t start_time, end_time;
  int problem;
  LONG prog;
  int prog2;
  double dense;
  char *input_fname;
  char *output_fname;
  char *weight_fname;
  char *table_fname, *table2_fname;
  char *position_fname, *position2_fname;

  char *sgraph_fname, *sgraph2_fname;
  char *sgraph_wfname, *sgraph2_wfname;
  char *agraph_fname, *agraph2_fname;
  char *trsact_fname, *trsact_fname2, *trsact_wfname, *trsact_wfname2, *trsact_pfname;
  char *trsact2_fname, *trsact2_fname2, *trsact2_wfname, *trsact2_wfname2, *trsact2_pfname;
  char *seq_fname, *seq2_fname;
  char *fstar_fname, *fstar2_fname;
  char *mat_fname, *mat2_fname;
  char *smat_fname, *smat2_fname;
  char *setfamily_fname, *setfamily2_fname;
  char *setfamily_wfname, *setfamily2_wfname;
  
  ITEMSET II, II2;
  QUEUE ff;      // for agraph search
  int *vf, *dep; // for agraph search

  int root, dir, edge_dir;
  double th, th2, th3;   // thresholds
  double ratio, ratio2;  // ratio
  int num, siz, dim, len;
  QUEUE_INT clms;
  VEC_ID rows;

  QUEUE_ID **shift;
  QUEUE itemjump, itemcand, vecjump, veccand, *OQ, *OQ2, *VQ, *VQ2;   // for delivery
  QUEUE_INT *itemary;
  int *itemmark, *itemflag, *vecmark, *vecflag;  // mark for vector
  VEC_ID *vecary, *occ_t;
  WEIGHT *occ_w, *occ_pw, *occ_w2, *occ_pw2;
  QUEUE oo;

  char *pat;   // pattern string
  int plen, perr;  // pattern length and #error allowed

#ifdef _alist_h_
  MALIST occ;
#endif

#ifdef _sgraph_h_
  SGRAPH SG, SG2;
#endif

#ifdef _agraph_h_
  AGRAPH AG, AG2;
#endif

#ifdef _trsact_h_
  TRSACT TT, TT2;
#endif

#ifdef _seq_h_
  SEQ SS, SS2;
#endif

#ifdef _fstar_h_
  FSTAR FS, FS2;
#endif

#ifdef _vec_h_
  MAT MM, MM2;
  SMAT SM, SM2;
  SETFAMILY FF, FF2;
#endif

} PROBLEM;


/*****  print filename information  ****/
void PROBLEM_print (PROBLEM *P);

/*****  print usage of the program *****/
void PROBLEM_error ();

/*****  read parameters given by command line  *****/
void PROBLEM_read_param (int argc, char *argv[], PROBLEM *P);

/*****  PROBLEM and ITEMSET initialization *****/
/* all pointers are set to NULL, but don't touch filenames */
void PROBLEM_init (PROBLEM *P);

/*****  PROBLEM initialization: load the files given by filenames   ******/
void PROBLEM_init2 (PROBLEM *P, int flag);

/*****  allocate memory according to flag  *****/
void PROBLEM_alloc (PROBLEM *PP, QUEUE_ID siz, QUEUE_ID siz2, size_t siz3, PERM *p, int f);

/* termination of problem */
void PROBLEM_end (PROBLEM *PP);


#endif


