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

#ifndef _base_c_
#define _base_c_

#include"base.h"

BASE INIT_BASE = {TYPE_BASE,NULL,0,0,0,0,-1,NULL};

/*  initialization, and allocate memory for header */
void BASE_alloc (BASE *B, int unit, int block_siz){
  *B = INIT_BASE;
  B->dellist = B;
  B->unit = unit;
  B->block_siz = block_siz;
  B->num = block_siz;
  B->block_num = -1;
  calloc2 (B->base, 20, "BASE_alloc: B->base", EXIT);
  B->block_end = 20;
}

/*  termination */
void BASE_end (BASE *B){
  int i;
  FLOOP (i, 0, B->block_end) free2 (B->base[i]);
  free2 (B->base);
  *B = INIT_BASE;
}

/* return pointer to the cell corresponding to the given index */
void *BASE_pnt (BASE *B, size_t i){
  return ( B->base[i/BASE_BLOCK] + (i%BASE_BLOCK)*B->unit);
}
/* return index corresponding to the given pointer */
size_t BASE_index (BASE *B, void *x){
  size_t i;
  FLOOP (i, 0, (size_t)(B->block_end+1)){
    if ( ((char*)x)>= B->base[i] && ((char*)x)<=B->base[i]+B->unit*BASE_BLOCK )
      return ( i*BASE_BLOCK + ((size_t)(((char *)x) - B->base[i])) / B->unit);
  }
  return (0);
}

/* increment the current memory block pointer and (re)allcate memory if necessary */
void *BASE_get_memory (BASE *B, int i){
  B->num += i;
  if ( B->num >= B->block_siz ){  /* if reach to the end of base array */
    B->num = i;  /* allocate one more base array, and increment the counter */
    B->block_num++;
    reallocx(B->base, B->block_end, B->block_num, NULL, "BASE:block", EXIT0);
    if ( B->base[B->block_num] == NULL )
        malloc2 (B->base[B->block_num], B->block_siz*B->unit, "BASE_new: base", EXIT0);
    return (B->base[B->block_num]);
  }
  return (B->base[B->block_num] + (B->num-i)*B->unit);
}


/* allocate new cell */
void *BASE_new (BASE *B){
  char *x;
  
    /* use deleted cell if it exists */
  if ( B->dellist != ((void *)B) ){
    x = (char *)B->dellist;  /* return the deleted cell */
    B->dellist = (void *)(*((char **)x));      /* increment the head of the list */
  } else {
     /* take a new cell from the base array if no deleted one exists */
    x = (char *)BASE_get_memory (B, 1);
  }
  return (x);
}

/* delete one cell. (add to the deleted list)  */
void BASE_del (BASE *B, void *x){
  *((void **)x) = B->dellist;
  B->dellist = x;
}

#endif


