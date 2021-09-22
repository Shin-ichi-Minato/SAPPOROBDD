/*
    blocked memory allocation library
            12/Mar/2002   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users.
   For the commercial use, please make a contact to Takeaki Uno. */


#ifndef _base_h_
#define _base_h_

#include"stdlib2.h"

/* structure for base array */
#define BASE_UNIT 16
#define BASE_BLOCK 65536

typedef struct {
  unsigned char type;
  char **base;
  int block_siz;  // size of one block of memory
  int block_num;  // currently using block
  int unit;  // size of one unit memory
  int num;   // current position in a block
  int block_end;  // current end of the block
  void *dellist;
} BASE;

extern BASE INIT_BASE;

/*  initialization, and allocate memory for header */
void BASE_alloc (BASE *B, int unit, int block_siz);

/*  termination */
void BASE_end (BASE *B);

/* return pointer to the cell corresponding to the given index */
void *BASE_pnt (BASE *B, size_t i);

/* return index corresponding to the given pointer */
size_t BASE_index (BASE *B, void *x);

/* increment the current memory block pointer and (re)allcate memory if necessary */
void *BASE_get_memory (BASE *B, int i);

/* allocate new cell */
void *BASE_new (BASE *B);

/* delete one cell. (add to the deleted list)  */
void BASE_del (BASE *B, void *x);


#endif

