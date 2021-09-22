/* library for vector and sparse vector, and matrix */
/* Takeaki Uno    27/Dec/2008 */

#ifndef _vec_c_
#define _vec_c_

#include"vec.h"
#include"stdlib2.c"
#include"queue.c"

MAT INIT_MAT = {TYPE_MAT,NULL,0,0,NULL,NULL,0,0,0};
SVEC INIT_SVEC_ELE = {0,0};
SVEC INIT_SVEC = {TYPE_SVEC,NULL,0,0};
SMAT INIT_SMAT = {TYPE_SMAT,NULL,0,0,NULL,NULL,0,0,0,0};
SETFAMILY INIT_SETFAMILY = INIT_SETFAMILY_;

QSORT_TYPE (SVEC_VAL, SVEC_VAL)
QSORT_TYPE (SVEC_VAL2, SVEC_VAL2)

/* allocate memory according to rows and rowt */
void VEC_alloc (VEC *V, VEC_ID clms){
  *V = INIT_VEC;
  V->end = clms;
  calloc2 (V->v, clms+1, "VEC_alloc: V->v", EXIT);
}

/* terminate routine for VEC */
void VEC_end (VEC *V){
  free2 (V->v);
  *V = INIT_VEC;
}

/* allocate memory according to rows and rowt */
void MAT_alloc (MAT *M, VEC_ID rows, VEC_ID clms){
  VEC_ID i;
  calloc2 (M->v, rows+1, "MAT_alloc: M->v", EXIT);
  calloc2 (M->buf, (clms+1) * (rows+1), "MAT_alloc: M->v", {free(M->v);EXIT;});
  M->end = rows;
  M->clms = clms;
  FLOOP (i, 0, rows){
    M->v[i].end = M->v[i].t = clms;
    M->v[i].v = M->buf + i*(clms+1);
  }
}

/* terminate routine for MAT */
void MAT_end (MAT *M){
  free2 (M->buf);
  free2 (M->buf2);
  free2 (M->v);
  *M = INIT_MAT;
}

/* allocate memory */
void SVEC_alloc (SVEC *V, VEC_ID end){
  *V = INIT_SVEC;
  calloc2 (V->v, end+1, "SVEC_alloc: V->v", EXIT);
  V->end = end;
  V->t = 0;
}

/* terminate routine for SVEC */
void SVEC_end (SVEC *V){
  free2 (V->v);
  *V = INIT_SVEC;
}

/* allocate memory according to rows and rowt */
void SMAT_alloc (SMAT *M, VEC_ID rows, VEC_ID *rowt, VEC_ID clms, size_t eles){
  VEC_ID i;
  if ( eles == 0 ) ARY_SUM (M->ele_end, rowt, 0, rows); else M->ele_end = eles;
  calloc2 (M->buf, M->ele_end*((M->flag&LOAD_DBLBUF)?2:1) +rows +2, "SMAT_alloc: buf", EXIT);
  malloc2 (M->v, rows+1, "SMAT_alloc: M->v", {free(M->buf);EXIT;});
  ARY_FILL (M->v, 0, rows, INIT_SVEC);
  M->end = rows;
  M->clms = clms;
  if ( rowt ){
    FLOOP (i, 0, rows){
      M->v[i].v = i? M->v[i-1].v + rowt[i-1] +1: M->buf;
      M->v[i].end = rowt[i];
    }
  }
}

/* terminate routine for MAT */
void SMAT_end (SMAT *M){
  free2 (M->buf);
  free2 (M->buf2);
  free2 (M->v);
  *M = INIT_SMAT;
}



/* allocate memory according to rows and rowt */
/* if eles == 0, compute eles from rowt and rows */
void SETFAMILY_alloc (SETFAMILY *M, VEC_ID rows, VEC_ID *rowt, VEC_ID clms, size_t eles){
  VEC_ID i;
  char *buf;
  if ( eles == 0 ) ARY_SUM (M->ele_end, rowt, 0, rows); else M->ele_end = eles;
  calloc2 (buf, (M->ele_end*((M->flag&LOAD_DBLBUF)?2:1) +((M->flag&LOAD_DBLBUF)?MAX(rows,clms):rows)+2)*M->unit, "SETFAMILY_alloc: buf", EXIT);
  M->buf = (QUEUE_INT *)buf;
  malloc2 (M->v, rows+1, "SETFAMILY_alloc: M->v", {free(M->buf);EXIT;});
  ARY_FILL (M->v, 0, rows, INIT_QUEUE);
  M->end = rows;
  M->clms = clms;
  if ( rowt ){
    FLOOP (i, 0, rows){
      M->v[i].v = (QUEUE_INT *)buf;
      buf += (rowt[i] +1)*M->unit;
      M->v[i].end = rowt[i]+1;
    }
  }
}

/* allocate memory according to rows and rowt */
/* if eles == 0, compute eles from rowt and rows */
void SETFAMILY_alloc_weight (SETFAMILY *M){
  VEC_ID i;
  calloc2 (M->w, M->end +1, "SETFAMILY_alloc_weight: w", EXIT);
  calloc2 (M->wbuf, M->ele_end*((M->flag&LOAD_DBLBUF)?2:1)+1, "SETFAMILY_alloc_weight: *w", {free(M->w);EXIT;});
  FLOOP (i, 1, M->t) M->w[i] = i? M->w[i-1] + M->v[i-1].t: M->wbuf;
}

/* terminate routine for MAT */
void SETFAMILY_end (SETFAMILY *M){
  free2 (M->buf);
  free2 (M->buf2);
  free2 (M->v);
  free2 (M->wbuf);
  free2 (M->w);
  *M = INIT_SETFAMILY;
}

/****************************************************************/
/****************************************************************/
/****************************************************************/

/* read binary file for MAT */
/* each unit-byte will be one number. if unit<0, the sign of unit is flipped, and each value is minesed the half of the maximum */
void MAT_load_bin (MAT *M, FILE2 *fp, int unit){
  VEC_ID flag=0, i, j, jj;
  size_t siz=0;
  VEC_VAL z, neg=0;

  if ( unit < 0 ){
    unit = -unit; flag = 1; neg=128;
    FLOOP (jj, 0, unit-1) neg *= 256;
  }
  if ( M->t == 0 ){  // determine #rows if M->t is 0 (not specified)
    fseek(fp->fp, 0, SEEK_END);
    siz = ftell(fp->fp);
    fseek(fp->fp, 0, SEEK_SET);
    M->t = (VEC_ID)(siz / unit / M->clms);
    if ( M->flag & LOAD_TPOSE ) SWAP_VEC_ID (M->t, M->clms);
  }
  MAT_alloc (M, M->t, M->clms);  if (ERROR_MES) return;
  M->end = M->t;
  FLOOP (i, 0, M->t){
    FLOOP (j, 0, M->clms){
      z=0; FLOOP (jj, 0, unit){ z *= 256; z += FILE2_getc (fp); }
      if ( flag ) z -= neg;
      if ( M->flag & LOAD_TPOSE ) M->v[j].v[i] = z;
      else M->v[i].v[j] = z;
    }
  }
}

/* segmentation fault for illegal files */
/* count/read the number in file for MAT */
/* if *rows>0, only read count the numbers in a row, for the first scan. */
void MAT_file_load (MAT *M, FILE2 *fp){
  QUEUE_ID c;
  VEC_ID t=0;
  double p;

  for ( t=0 ; (FILE_err&2)==0 ; t++){
    ARY_SCAN (c, double, *fp, 0);
    if ( M->flag & LOAD_TPOSE ){
      if ( M->t == 0 ){ M->t = c; if ( M->clms>0 ) break; }
    } else if ( M->clms == 0 ){ M->clms = c; if ( M->t>0 ) break; }
  }
  if ( M->flag & LOAD_TPOSE ){ if ( M->clms==0 ) M->clms = t;} else if ( M->t==0 ) M->t = t;
  FILE2_reset (fp);
  M->end = M->t;
  MAT_alloc (M, M->t, M->clms); if (ERROR_MES) return;
  FLOOP (t, 0, M->t ){
    FLOOP (c, 0, M->clms){
      p = FILE2_read_double(fp);
      if ( M->flag&LOAD_TPOSE ) M->v[c].v[t] = p;
      else M->v[t].v[c] = p;
      if ( c>= ((M->flag&LOAD_TPOSE)? M->t: M->clms) ) break;
    }
    if ( !FILE_err ) FILE2_read_until_newline (fp);
    if ( c>= ((M->flag&LOAD_TPOSE)? M->clms: M->t) ) break;
  }
}

/* load file with switching the format according to the flag */
void MAT_load (MAT *M, char *fname){
  FILE2 fp;
  int unit=0;
#ifdef USE_MATH
  VEC_ID i;
#endif
  if ( M->flag & VEC_LOAD_BIN ) unit = 1;
  else if ( M->flag & VEC_LOAD_BIN2 ) unit = 2;
  else if ( M->flag & VEC_LOAD_BIN4 ) unit = 4;
  if ( M->flag & VEC_LOAD_CENTERIZE ) unit = -unit;

  FILE2_open (fp, fname, "rb", "MAT_load", EXIT);
  if ( unit ) MAT_load_bin (M, &fp, unit);
  else MAT_file_load (M, &fp);
  FILE2_close (&fp); if (ERROR_MES) EXIT;
#ifdef USE_MATH
  if ( M->flag&VEC_NORMALIZE ) FLOOP (i, 0, M->t) ARY_NORMALIZE (M->v[i].v,M->v[i].t);
#endif
}


/* scan file and read the numbers for SMAT */
/* flag&1? SMAT, SETFAMILY,  flag&2? tuple list format: array list :*/ 
void SMAT_file_load (SMAT *M, FILE2 *fp){
  SVEC_VAL z=0;
  VEC_ID flag= (M->type==TYPE_SMAT), t, x, y;
  FILE_COUNT C;

  C = FILE2_count (fp, (M->flag&(LOAD_ELE+LOAD_TPOSE)) | FILE_COUNT_ROWT, 0, 0, 0, 0, 0);
  if ( M->clms == 0 ) M->clms = C.clms;
  if ( M->t == 0 ) M->t = C.rows;
  if ( flag ) SMAT_alloc (M, M->t, C.rowt, M->clms, 0);
  else SETFAMILY_alloc ((SETFAMILY *)M, M->t, C.rowt, M->clms, 0);
  free2 (C.rowt);
  if ( ERROR_MES ) return;
  FILE2_reset (fp);
  t=0;
  do {
    if ( M->flag&LOAD_ELE ){
      x = (VEC_ID)FILE2_read_int (fp);
      y = (VEC_ID)FILE2_read_int (fp);
      if ( flag ) z = FILE2_read_double (fp);
      if ( FILE_err&4 ) goto LOOP_END2;
      FILE2_read_until_newline (fp);
    } else {
      x = t;
      y = (VEC_ID)FILE2_read_int (fp);
      if ( FILE_err&4 ) goto LOOP_END2;
      if ( flag ) z = FILE2_read_double (fp);
    }
    if ( M->flag&LOAD_TPOSE ) SWAP_VEC_ID (x, y);
//  printf ("%d %d       %d %d\n", x, M->t, y, M->clms);
    if ( y >= M->clms || x >= M->t ) goto LOOP_END2;
//  printf ("## %d %d\n", x, y);
    if ( flag ){
      M->v[x].v[M->v[x].t].i = y;
      M->v[x].v[M->v[x].t].a = z;
      M->v[x].t++;
    } else ARY_INS (((SETFAMILY *)M)->v[x], y);
    LOOP_END2:;
    if ( !(M->flag&LOAD_ELE) && (FILE_err&3) ){ t++; if ( t >= M->t ) break; }
  } while ( (FILE_err&2)==0 );
}

/* scan file and read the numbers for SMAT */
/* flag&1? SMAT, SETFAMILY,  flag&2? tuple list format: array list :*/ 
void SETFAMILY_load_weight (SETFAMILY *M, char *fname){
  FILE2 fp;
  VEC_ID i;
  QUEUE_ID j;
  if ( M->flag&LOAD_TPOSE ) error ("transope and weight can't be specified simultaneously", EXIT);
  FILE2_open (fp, fname, "r", "SETFAMILY_load_weight", EXIT);
  SETFAMILY_alloc_weight (M);
  FLOOP (i, 0, M->t){
    FLOOP (j, 0, M->v[i].t)
        M->w[i][j] = (WEIGHT)FILE2_read_double (&fp);
    FILE2_read_until_newline (&fp);
  }
}


/* load file with switching the format according to the flag */
void SMAT_load (SMAT *M, char *fname){
  FILE2 fp;
  VEC_ID i;
  M->type = TYPE_SMAT;
  FILE2_open (fp, fname, "r", "SMAT_load", EXIT);
  SMAT_file_load (M, &fp);
  FILE2_close (&fp);    if (ERROR_MES) EXIT;
  FLOOP (i, 0, M->t) M->v[i].v[M->v[i].t].i = M->clms;  // end mark

#ifdef USE_MATH
  if ( M->flag&VEC_NORMALIZE ) FLOOP (i, 0, M->t) SVEC_normalize (&M->v[i]); // normalize
#endif
  if (M->flag&LOAD_INCSORT)
      FLOOP (i, 0, M->t) qsort_VEC_ID ((VEC_ID *)(M->v[i].v), M->v[i].t, sizeof(SVEC_ELE));
  if (M->flag&LOAD_DECSORT)
      FLOOP (i, 0, M->t) qsort_VEC_ID ((VEC_ID *)(M->v[i].v), M->v[i].t, -(int)sizeof(SVEC_ELE));
  if (M->flag&LOAD_RM_DUP)
      FLOOP (i, 0, M->t) MQUE_UNIFY (M->v[i], SVEC_VAL);
  M->eles = M->ele_end;
}

/* sort and duplication check */
void SETFAMILY_sort (SETFAMILY *M){
  VEC_ID i;
  PERM *p;
  WEIGHT *ww;
  QUEUE Q;
  int flag = (M->flag&LOAD_INCSORT)? 1: ((M->flag&LOAD_DECSORT)? -1: 0);
  if ( flag ){   // sort items in each row
    malloc2 (p, M->clms, "SETFAMILY_sort: p", EXIT);
    FLOOP (i, 0, M->t)
        QUEUE_perm_WEIGHT (&M->v[i], M->w?M->w[i]:NULL, p, flag);
    free (p);
  }
  flag = ((M->flag&LOAD_SIZSORT)? ((M->flag&LOAD_DECROWSORT)? -1: 1): 0) *sizeof(QUEUE);
  if ( flag ){   // sort the rows
    p = qsort_perm_VECt ((VEC *)M->v, M->t, flag);
    ARY_INVPERMUTE_ (M->w, p, ww, M->t);
    ARY_INVPERMUTE (M->v, p, Q, M->t, "SETFAMILY_sort: ARY_INVPERMUTE", EXIT);
    free (p);
  }
  if (M->flag&LOAD_RM_DUP){  // unify the duplicated edges
    FLOOP (i, 0, M->t)
        QUEUE_rm_dup_WEIGHT (&M->v[i], M->w?M->w[i]:NULL);
  }
}

/* scan file and load the data from file to SMAT structure */
void SETFAMILY_load (SETFAMILY *M, char *fname, char *wfname){
  FILE2 fp;
  VEC_ID i;
  M->type = TYPE_SETFAMILY;
  FILE2_open (fp, fname, "r", "SETFAMILY_load", EXIT);
  SMAT_file_load ((SMAT *)M, &fp);
  FILE2_close (&fp);     if(ERROR_MES) EXIT;
  FLOOP (i, 0, M->t) M->v[i].v[M->v[i].t] = M->clms;  // end mark

  if ( !(M->flag&LOAD_ELE) && wfname ){
    SETFAMILY_load_weight (M, wfname);
    if ( ERROR_MES ){ SETFAMILY_end (M); EXIT; }
  }
 
  SETFAMILY_sort (M);
  M->eles = M->ele_end;
}

/* print routines */
void MAT_print (FILE *fp, MAT *M){
  VEC *V;
  MQUE_FLOOP (*M, V) ARY_FPRINT (fp, V->v, 0, V->t, VEC_VALF" ");
}
void SVEC_print (FILE *fp, SVEC *V){
  SVEC_ELE *x;
  MQUE_FLOOP (*V, x) fprintf (fp, "("QUEUE_IDF","SVEC_VALF") ", (*x).i, (*x).a);
  fputc ('\n', fp);
}
void SMAT_print (FILE *fp, SMAT *M){
  SVEC *V;
  MQUE_FLOOP (*M, V) SVEC_print (fp, V);
}
void SETFAMILY_print (FILE *fp, SETFAMILY *M){
  QUEUE *V;
  MQUE_FLOOP (*M, V) ARY_FPRINT (fp, V->v, 0, V->t, QUEUE_INTF" ");
}

/*
void SETFAMILY_print_WEIGHT (FILE *fp, SETFAMILY *M){
  if ( M->w ){
     printf (","); fprint_WEIGHT (stdout, M->w[i][j]); }
  printf ("\n");
}
*/

/****************************************************************/
/** Inner product routines **************************************/
/****************************************************************/
SVEC_VAL2 SVEC_inpro (SVEC *V1, SVEC *V2){
  VEC_ID i1, i2=0;
  SVEC_VAL2 sum=0;
  FLOOP (i1, 0, V1->t){
    while (V2->v[i2].i < V1->v[i1].i) i2++;
    if (V2->v[i2].i == V1->v[i1].i) sum += ((SVEC_VAL2)V2->v[i2].a)*V1->v[i1].a;
  }
  return (sum);
}


/* get ith vector */
void *MVEC_getvec (void *M, int i, int flag){
  MAT *MM = (MAT *)M;
  if (MM->type==TYPE_MAT) return (&MM->v[i]);
  if (MM->type==TYPE_SMAT) return (&((SVEC *)M)->v[i]);
  if (MM->type==TYPE_SETFAMILY) return (&((QUEUE *)M)->v[i]);
  return (NULL);
}

#ifdef USE_MATH

/****************************************************************/
/** Norm computation and normalization   ************************/
/****************************************************************/
double SVEC_norm (SVEC *V){
  SVEC_ELE *v;
  double sum=0;
  MQUE_FLOOP (*V, v) sum += ((double)(v->a)) * (v->a);
  return (sqrt(sum));
}
void SVEC_normalize (SVEC *V){
  SVEC_ELE *v;
  double norm = SVEC_norm (V);
  MQUE_FLOOP (*V, v) v->a /= norm;
}

/****************************************************************/
/** Euclidean distance routines *********************************/
/****************************************************************/

/* compute the inner product of two vectors */
double VEC_eucdist (VEC *V1, VEC *V2){
  VEC_ID i, end=MIN(V1->end,V2->end);
  double sum=0, a0, a1, a2, a3;
  for (i=0 ; i<end ; i+=4){
    a0 = ((double)V1->v[i])- ((double)V2->v[i]);
    a1 = ((double)V1->v[i+1])- ((double)V2->v[i+1]);
    a2 = ((double)V1->v[i+2])- ((double)V2->v[i+2]);
    a3 = ((double)V1->v[i+3])- ((double)V2->v[i+3]);
    sum += a0*a0 + a1*a1 + a2*a2 + a3*a3;
  }
  if ( i+1<end ){
    a0 = ((double)V1->v[i])- ((double)V2->v[i]);
    a1 = ((double)V1->v[i+1])- ((double)V2->v[i+1]);
    sum += a0*a0 + a1*a1;
    if ( i+2<end ){ a2 = ((double)V1->v[i+2])- ((double)V2->v[i+2]); sum += a2*a2; }
  } else if ( i<end ){ a0 = ((double)V1->v[i])- ((double)V2->v[i]); sum += a0*a0; }
  return (sqrt(sum));
}

/* compute the inner product of two vectors */
double SVEC_eucdist (SVEC *V1, SVEC *V2){
  VEC_ID i1, i2;
  double sum=0, a;
  for ( i1=i2=0 ; i1<V1->t && i2<V2->t ; ){
    if (V2->v[i2].i > V1->v[i1].i) a = V1->v[i1].a;
    else if (V2->v[i2].i < V1->v[i1].i) a = V2->v[i2].a;
    else a = ((double)V2->v[i2].a) - ((double)V1->v[i1].a);
    sum += a*a;
  }
  return (sqrt(sum));
}

/* compute the inner product of two vectors */
double VEC_SVEC_eucdist (VEC *V1, SVEC *V2){
  VEC_ID i, i2=0;
  double sum=0, a;
  FLOOP (i, 0, V1->end){
    if ( i < V2->v[i2].i ) a = V1->v[i];
    else { a = ((double)V1->v[i]) - ((double)V2->v[i2].a); i2++; }
    sum += a*a;
  }
  return (sqrt(sum));
}

/**********************************************************/
/* Euclidean distance of vector and set */
double VEC_QUEUE_eucdist (VEC *V, QUEUE *Q){
  VEC_ID i;
  QUEUE_ID i2=0;
  double sum=0, a;
  FLOOP (i, 0, V->end){
    if ( i < Q->v[i2] ) a = V->v[i];
    else { a = ((double)V->v[i]) - 1.0; i2++; }
    sum += a*a;
  }
  return (sqrt(sum));
}

/* compute Euclidean distance of two sets */
double QUEUE_eucdist (QUEUE *Q1, QUEUE *Q2){
  double f;
  MQUE_UNION(f, *Q1, *Q2);
  return (sqrt(f));
}

double MVEC_norm (void *V){
  VEC *VV = (VEC *)V;
  double p;
  if (VV->type==TYPE_VEC){ ARY_NORM (p, VV->v, VV->t); return (p); }
  if (VV->type==TYPE_SVEC) return (SVEC_norm ((SVEC *)V));
  if (VV->type==TYPE_QUEUE) return (sqrt(((QUEUE*)V)->t));
  return (0.0);
}

double MMAT_norm_i (void *M, int i){
  MAT *MM = (MAT *)M;
  double p;
  if (MM->type==TYPE_MAT){ ARY_NORM (p, MM->v[i].v, MM->v[i].t); return (p); }
  if (MM->type==TYPE_SMAT) return (SVEC_norm (&((SMAT *)M)->v[i]));
  if (MM->type==TYPE_SETFAMILY) return (sqrt (((SETFAMILY *)M)->v[i].t));
  return (0.0);
}

double MVEC_eucdist (void *V, void *U){
  VEC *VV = (VEC *)V;
  double p;
  if (VV->type==TYPE_VEC) return (VEC_eucdist ((VEC *)V, (VEC *)U));
  if (VV->type==TYPE_SVEC) return (SVEC_eucdist ((SVEC *)V, (SVEC *)U));
  if (VV->type==TYPE_QUEUE){ MQUE_DIF (p, *((QUEUE *)V), *((QUEUE *)U)); return (sqrt(p));}
  return (0.0);
}

double MMAT_eucdist_ij (void *M, int i, int j){
  MAT *MM=(MAT *)M;
  double p;
  if (MM->type==TYPE_MAT) return (VEC_eucdist ( &MM->v[i], &MM->v[j] ));
  if (MM->type==TYPE_SMAT) return (SVEC_eucdist ( &((SMAT *)M)->v[i], &((SMAT *)M)->v[j]));
  if (MM->type==TYPE_SETFAMILY){ MQUE_DIF (p, ((SETFAMILY *)M)->v[i], ((SETFAMILY *)M)->v[j]); return (sqrt(p)); }
  return (0.0);
}


#endif

/**********************************************************/
/**   multi-vector routines  ******************************/
/**********************************************************/

/* compute the inner product, Euclidean distance for multi vector */
double MVEC_inpro (void *V, void *U){
  VEC *VV = (VEC *)V, *UU = (VEC *)U;
  double p;
  if (VV->type==TYPE_VEC){
    if (UU->type==TYPE_VEC){ ARY_INPRO (p, VV->v, UU->v, VV->t); return (p); }
    if (UU->type==TYPE_SVEC){ ARY_SVEC_INPRO (p, *((SVEC *)U), VV->v); return (p); }
    if (UU->type==TYPE_QUEUE){ ARY_QUEUE_INPRO (p, *((QUEUE *)U), VV->v); return (p); }
  }
  if (VV->type==TYPE_SVEC){
    if (UU->type==TYPE_VEC){ ARY_SVEC_INPRO (p, *((SVEC *)V), UU->v); return (p);}
    if (UU->type==TYPE_SVEC) return (SVEC_inpro ((SVEC *)V, (SVEC *)U));
//  if (UU->type==TYPE_QUEUE) return (VEC_QUEUE_inpro (V, U));
  }
  if (VV->type==TYPE_QUEUE){
    if (UU->type==TYPE_VEC){ ARY_QUEUE_INPRO (p, *((QUEUE *)V), UU->v); return (p); }
//    else if (UU->type==TYPE_SVEC) return (SVEC_inpro (V, U));
    if (UU->type==TYPE_QUEUE){ MQUE_INTSEC (p, *((QUEUE *)V), *((QUEUE *)U)); return (p);}
  }
  return (0.0);
}

double MVEC_double_inpro (void *V, double *w){
  VEC *VV = (VEC *)V;
  double p;
  if (VV->type==TYPE_VEC){ ARY_INPRO (p, VV->v, w, VV->t); return (p); }
  if (VV->type==TYPE_SVEC){ ARY_SVEC_INPRO (p, *((SVEC *)V), w); return (p); }
  if (VV->type==TYPE_QUEUE){ ARY_QUEUE_INPRO (p, *((QUEUE *)V), w); return (p); }
  return (0.0);
}

/* compute the inner product, euclidean distance for i,jth vector */
double MMAT_inpro_ij (void *M, int i, int j){
  MAT *MM = (MAT *)M;
  double p;
  if (MM->type==TYPE_MAT){ ARY_INPRO (p, MM->v[i].v, MM->v[j].v, MM->v[j].t); return (p); }
  if (MM->type==TYPE_SMAT) return (SVEC_inpro (&((SMAT *)M)->v[i], &((SMAT *)M)->v[j]));
  if (MM->type==TYPE_SETFAMILY){
     p = QUEUE_intsec_ (&((SETFAMILY *)M)->v[i], &((SETFAMILY *)M)->v[j]); return (p); }
  return (0.0);
}

double MMAT_double_inpro_i (void *M, int i, double *w){
  MAT *MM = (MAT *)M;
  double p;
  if (MM->type==TYPE_MAT){ ARY_INPRO (p, MM->v[i].v, w, MM->v[i].t); return (p); }
  if (MM->type==TYPE_SMAT){ ARY_SVEC_INPRO (p, ((SMAT *)M)->v[i], w); return (p); }
  if (MM->type==TYPE_SETFAMILY){ ARY_QUEUE_INPRO (p, ((SETFAMILY *)M)->v[i], w); return (p); }
  return (0.0);
}



#endif


