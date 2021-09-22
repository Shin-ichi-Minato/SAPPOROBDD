/* library for standard macros and functions */
/* by Takeaki Uno 2/22/2002, e-mail: uno@nii.jp
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users.
   For the commercial use, please make a contact to Takeaki Uno. */

#ifndef _stdlib2_c_
#define _stdlib2_c_

#include"stdlib2.h"
#ifdef MTWISTER
#include"dSFMT.c"
#endif

size_t common_size_t;
INT common_INT, common_INT2;
char *common_charp, *common_pnt;
FILE *common_FILE;
FILE2 common_FILE2;
PERM common_PERM;

char *ERROR_MES = NULL;
int print_time_flag=0;
PARAMS internal_params;
#ifdef MULTI_CORE
int SPIN_LOCK_dummy;
#endif
FILE2 INIT_FILE2 = {TYPE_FILE2,NULL,NULL,NULL,NULL};
VEC INIT_VEC = {TYPE_VEC,NULL,0,0};
FILE_COUNT INIT_FILE_COUNT = {0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,0,0,NULL,NULL,0,0,NULL,NULL};

QSORT_TYPE (int, int)
QSORT_TYPE (uint, unsigned int)
QSORT_TYPE (double, double)
QSORT_TYPE (char, char)
QSORT_TYPE (uchar, unsigned char)
QSORT_TYPE (short, short)
QSORT_TYPE (ushort, unsigned short)
QSORT_TYPE (WEIGHT, WEIGHT)
QSORT_TYPE (LONG, LONG)
QSORT_TYPE (VEC_ID, VEC_ID)
QSORT_TYPE (VEC_VAL, VEC_VAL)
QSORT_TYPE (VEC_VAL2, VEC_VAL2)
QSORT_TYPE (FILE_COUNT_INT, FILE_COUNT_INT)

  /* bitmasks, used for bit operations */
int BITMASK_UPPER1[32] = { 0xffffffff, 0xfffffffe, 0xfffffffc, 0xfffffff8,
                           0xfffffff0, 0xffffffe0, 0xffffffc0, 0xffffff80,
                           0xffffff00, 0xfffffe00, 0xfffffc00, 0xfffff800,
                           0xfffff000, 0xffffe000, 0xffffc000, 0xffff8000,
                           0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000,
                           0xfff00000, 0xffe00000, 0xffc00000, 0xff800000,
                           0xff000000, 0xfe000000, 0xfc000000, 0xf8000000,
                           0xf0000000, 0xe0000000, 0xc0000000, 0x80000000 };
int BITMASK_UPPER1_[32] = { 0xfffffffe, 0xfffffffc, 0xfffffff8, 0xfffffff0,
                            0xffffffe0, 0xffffffc0, 0xffffff80, 0xffffff00,
                            0xfffffe00, 0xfffffc00, 0xfffff800, 0xfffff000,
                            0xffffe000, 0xffffc000, 0xffff8000, 0xffff0000,
                            0xfffe0000, 0xfffc0000, 0xfff80000, 0xfff00000,
                            0xffe00000, 0xffc00000, 0xff800000, 0xff000000,
                            0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000,
                            0xe0000000, 0xc0000000, 0x80000000, 0x00000000 };

int BITMASK_LOWER1[32] = { 0x00000000, 0x00000001, 0x00000003, 0x00000007,
                           0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
                           0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
                           0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
                           0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
                           0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
                           0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
                           0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff };
int BITMASK_LOWER1_[32] = { 0x00000001, 0x00000003, 0x00000007, 0x0000000f,
                            0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
                            0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
                            0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
                            0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
                            0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
                            0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
                            0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff };

int BITMASK_1[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008,
                      0x00000010, 0x00000020, 0x00000040, 0x00000080,
                      0x00000100, 0x00000200, 0x00000400, 0x00000800,
                      0x00001000, 0x00002000, 0x00004000, 0x00008000,
                      0x00010000, 0x00020000, 0x00040000, 0x00080000,
                      0x00100000, 0x00200000, 0x00400000, 0x00800000,
                      0x01000000, 0x02000000, 0x04000000, 0x08000000,
                      0x10000000, 0x20000000, 0x40000000, 0x80000000 };
int BITMASK_31[32] = { 0xfffffffe, 0xfffffffd, 0xfffffffb, 0xfffffff7,
                       0xffffffef, 0xffffffdf, 0xffffffbf, 0xffffff7f,
                       0xfffffeff, 0xfffffdff, 0xfffffbff, 0xfffff7ff,
                       0xffffefff, 0xffffdfff, 0xffffbfff, 0xffff7fff,
                       0xfffeffff, 0xfffdffff, 0xfffbffff, 0xfff7ffff,
                       0xffefffff, 0xffdfffff, 0xffbfffff, 0xff7fffff,
                       0xfeffffff, 0xfdffffff, 0xfbffffff, 0xf7ffffff,
                       0xefffffff, 0xdfffffff, 0xbfffffff, 0x7fffffff };

int BITMASK_16[8] = { 0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000,
                      0x000f0000, 0x00f00000, 0x0f000000, 0xf0000000 };
int BITMASK_UPPER16[8] = { 0xffffffff, 0xfffffff0, 0xffffff00, 0xfffff000,
                           0xffff0000, 0xfff00000, 0xff000000, 0xf0000000 };
int BITMASK_LOWER16[8] = { 0x0000000f, 0x000000ff, 0x00000fff, 0x0000ffff,
                           0x000fffff, 0x00ffffff, 0x0fffffff, 0xffffffff };
int BITMASK_FACT16[8] = { 0x1, 0x10, 0x100, 0x1000,
                          0x10000, 0x100000, 0x1000000,0x10000000 };

/* free many pointers */
void mfree_(void *x, ...){
  va_list argp;
  void *a;
  va_start (argp, x);
  while((a = va_arg(argp, void *)) != (void*)1){ free (a); }
  va_end (argp);
}

/* compute the minimum prime no less than n */
#define MINPRIME_END 6000
LONG min_prime (LONG n){
  LONG i, j=30, k;
  char f[MINPRIME_END];
  while(1) {
    FLOOP (i, 0, j) f[i]=0;
    for ( i=3 ; i*i < n+j ; i+=2 )
        for ( k=((n+i-1)/i)*i ; k<i+j ; k+=i ) f[(k-n)/2] = 1;
    FLOOP (i, 0, j) if ( f[i] == 0 ) return (n+ i*2+1);
    j *= 2;
  }
}

/* decompose the string by separator, and set v[i] to each resulted string.
  consecutive separators are regarded as one separator. */
/* string s has to have end mark 0 at the end */
/* original string s will be written, so that each separator will be end mark 0 */
/* at most [num] strings will be generated */
int string_decompose ( char **v, char *s, char sep, int max){
  int i=0;
  char *ss = s;
  do {
    while (*ss == sep) ss++;
    if ( *ss == 0 ) break;
    v[i++] = ss;
    while (*ss != sep){
      if ( *ss == 0 ) break;
      ss++;
    }
    if ( *ss == 0 ) break;
    *ss = 0;
    ss++;
  } while ( i<max);
  return (i);
}

unsigned long xor128(){ 
    static unsigned long x=123456789,y=362436069,z=521288629,w=88675123; 
    unsigned long t; 
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) ); 
}

/***********************************************************************/
/***********************************************************************/
#ifdef USE_MATH
#define NORMAL_RAND_BASE 2147483648LL

/* make two random numbers under normal distribution N(0,1) */
void rand_mk_2normal (double *a, double *b){
  double r1, r2;
  do {
    r1 = RAND1;
  } while (r1==0);
  r2 = RAND1;
  r1 = sqrt(-log(r1)*2);
  r2 *=  2*PI;
  *a  = r1*sin(r2);
  *b  = r1*cos(r2);
}

/* make a random point on a supersphere of d-dim., and set to double array already allocated */
void rand_d_gaussian (double *p, int d){
  int i;
  double a, b;
  for (i=0 ; i<d ; i+=2){
    rand_mk_2normal ( &a, &b);
    p[i] = a;
    if ( i+1 < d ) p[i+1] = b;
  }
}
void rand_sphere (double *p, int d){
  rand_d_gaussian (p, d);
  ARY_NORMALIZE (p, d);
}
#endif

/********************  file I/O routines  ********************************/

int FILE_err;  /*  signals  0: for normal termination
                   1: read a number, then encountered a newline,
                   2: read a number, then encountered the end-of-file
                   5: read no number, and encountered a newline
                   6: read no number, and encountered the end-of-file */


void FILE2_flush (FILE2 *fp){
  if ( fp->buf > fp->buf_org ){
    fwrite ( fp->buf_org, fp->buf-fp->buf_org, 1, fp->fp);
    fp->buf = fp->buf_org;
  }
}
void FILE2_close (FILE2 *fp){
  fclose2 (fp->fp);
  free2 (fp->buf_org);
}
void FILE2_closew (FILE2 *fp){
  FILE2_flush (fp);
  fclose2 (fp->fp);
  free2 (fp->buf_org);
}
void FILE2_reset (FILE2 *fp){
  fp->buf = fp->buf_org;
  fp->buf_end = fp->buf_org-1;
  fseek (fp->fp, 0, SEEK_SET);
}
/* fast file routine, getc, putc, puts, */
int FILE2_getc (FILE2 *fp){
  int c;
  if ( fp->buf >= fp->buf_end ){
    if ( (fp->buf_end < fp->buf_org+FILE2_BUFSIZ) && (fp->buf_end>=fp->buf_org) ) return (-1);
    fp->buf = fp->buf_org;
    fp->buf_end = fp->buf_org + fread (fp->buf, 1, FILE2_BUFSIZ, fp->fp);
    return (FILE2_getc (fp));
  }
  c = (unsigned char)(*(fp->buf));
  fp->buf++;
  return (c);
}
void FILE2_puts (FILE2 *fp, char *s){
  while ( *s != 0 ){
    *(fp->buf) = *s;
    s++;
    fp->buf++;
  }
}
void FILE2_putc (FILE2 *fp, char c){
  *(fp->buf) = c;
  fp->buf++;
}
/* fast file routine, print number, c is the char to be printed preceding to the number
   if c==0, nothing will be printed preceding the number
   if len<0 then the #digits following '.' does not change (filed by '0')  */
void FILE2_print_int (FILE2 *fp, LONG n, char c){
  LONG nn = n;
  char *s;
  if ( c ) FILE2_putc ( fp, c);
  if ( n == 0 ){ *(fp->buf) = '0'; fp->buf++; return; }
  if ( n < 0 ){ *(fp->buf) = '-'; fp->buf++; n = -n; }
  while ( nn>0 ){ nn /= 10; fp->buf++; }
  s = fp->buf-1;
  *(fp->buf) = 0;
  while ( n>0 ){ *s = '0'+(char)(n%10); s--; n/=10; }
}
/******/
void FILE2_print_real (FILE2 *fp, double n, int len, char c){
  int i=0, flag=1;
  double j=1;
  char *back;

  if ( c ) FILE2_putc (fp, c);
  if ( n<0 ){ FILE2_putc (fp, '-'); n *= -1; }
  while ( n >= j ) j*=10;
  if ( j==1 ){ *(fp->buf) = '0'; fp->buf++; }
  else while ( j>1 ){
    j /= 10;
    i = (int)(n/j);
    *(fp->buf) = '0'+i;
    n -= j*i;
    fp->buf++;
  }
  *(fp->buf) = '.'; back = fp->buf;
  fp->buf++;
  if ( len<0 ){ len = -len; flag = 0; }
  for ( ; len>0 ; len--){
    n *= 10.0;
    i = (int)n;
    *(fp->buf) = '0'+i;
    n -= i;
    fp->buf++;
    if ( i>0 ) back = fp->buf;
  }
  if ( flag ) fp->buf = back;
}
/******/
void FILE2_print_WEIGHT (FILE2 *fp, WEIGHT w, int len, char c){
#ifdef WEIGHT_DOUBLE
  FILE2_print_real(fp, w, len, c);
#else
  FILE2_print_int(fp, w, c);
#endif
}

/* Read an integer/a double from the file and return it,
    with read through the non-numeric letters.
   If it reaches to the end-of-file, then set FILE_err=2, if it reads a 
   newline, then set FILE_err=1.
   If read either the end-of-file or newline before reading an integer,
   return 5, and 6 */
FILE_LONG FILE2_read_int (FILE2 *fp){
  FILE_LONG item;
  int flag =1;
  int ch;
  FILE_err = 0;
  do {
    ch = FILE2_getc(fp);
    if ( ch == '\n' ){ FILE_err = 5; return (-INTHUGE); }
    if ( ch < 0 ){ FILE_err = 6; return (-INTHUGE); }
    if ( ch=='-' ) flag = -1;
  } while ( ch<'0' || ch>'9' );
  for ( item=(int)(ch-'0') ; 1 ; item=item*10 +(int)(ch-'0') ){
    ch = FILE2_getc(fp);
    if ( ch == '\n' ){ FILE_err = 1; return (flag*item); }
    if ( ch < 0 ){ FILE_err = 2; return (flag*item); }
    if ( (ch < '0') || (ch > '9')) return (flag*item);
  }
}
double FILE2_read_double (FILE2 *fp){
  double item, geta=1;
  int sign=1, ch;
  FILE_err = 0;
  while (1){
    ch = FILE2_getc(fp);
    if ( ch < 0 ){ FILE_err = 6; return (-DOUBLEHUGE); }
    if ( ch == '\n' ){ FILE_err = 5; return (-DOUBLEHUGE); }
    if ( ch=='-' ) sign *= -1;
    else if ( ch=='.' ) geta = 0.1;
    else if ( ch>='0' && ch<='9' ) break;
    else { sign = 1; geta = 1; }
  }

  item = geta * (ch-'0');
  if ( geta < 1.0 ) geta *= .1;
  while (1){
    ch = FILE2_getc(fp);
    if ( ch == '\n' ){ FILE_err = 1; return (item*sign); }
    if ( ch<0 ){ FILE_err = 2; return (item*sign); }
    if ( ch == '.' ) geta = .1;
    else if ( (ch < '0') || (ch > '9')) return (item*sign);
    else if ( geta < 1.0 ){
      item += geta*(ch-'0');
      geta *= 0.1;
    } else item = item*10 + (ch-'0');
  }
}

/* read a WEIGHT from file */
WEIGHT FILE2_read_WEIGHT (FILE2 *fp){
#ifdef WEIGHT_DOUBLE
  return (FILE2_read_double(fp));
#else
  return ((WEIGHT)FILE2_read_int(fp));
#endif
}

/* read through the file until newline or EOF */
void FILE2_read_until_newline (FILE2 *fp){
  int ch;
  if (FILE_err & 3) return;
  while (1){
    ch = FILE2_getc(fp);
    if ( ch == '\n' ){ FILE_err = 5; return; }
    if ( ch < 0 ){ FILE_err = 6; return; }
  }
}

void FILE2_printf (FILE2 *fp, char *mes, ...){
  va_list argp;
  va_start (argp, mes);
  sprintf (fp->buf, mes, argp);
  va_end (argp);
}

/* print a real number in a good style */
void fprint_real (FILE *fp, double f){
  char s[200];
  size_t i;
  i = sprintf (s, "%f", f);
  while ( s[i-1] == '0' ) i--;
  if ( s[i-1] == '.' ) i--;
  s[i] = 0;
  fprintf (fp, s);
}
void print_real (double f){
  fprint_real (stdout, f);
}

void fprint_WEIGHT (FILE *fp, WEIGHT f){
#ifdef WEIGHT_DOUBLE
  fprint_real (fp, f);
#else
  fprintf (fp, "%d", f);
#endif
}
void print_WEIGHT (WEIGHT f){
  fprint_WEIGHT (stdout, f);
}

/* count the clms, rows, items, each row size, each column size  */
/* file types can be, array list and element list*/
/* support transpose */
FILE_COUNT FILE2_count (FILE2 *fp, int flag, int skip_rows, int int_rows, int skip_clms, int int_clms, FILE_COUNT_INT row_limit){
  FILE_COUNT_INT k=0, j, x, y, t=0;
  int fr = flag&FILE_COUNT_ROWT, fc = flag&FILE_COUNT_CLMT;
  int fe = flag&LOAD_ELE, ft = flag&LOAD_TPOSE;
  FILE_COUNT C = INIT_FILE_COUNT;
  C.flag = flag;

  FLOOP (j, 0, skip_rows) FILE2_read_until_newline (fp);
  if ( flag & (FILE_COUNT_NUM+FILE_COUNT_GRAPHNUM) ){
    C.clms = (FILE_COUNT_INT)FILE2_read_int (fp);
    C.rows = (flag & FILE_COUNT_NUM)? (FILE_COUNT_INT)FILE2_read_int (fp): C.clms;
    C.eles = (FILE_COUNT_INT)FILE2_read_int (fp);
    if ( !(flag & (FILE_COUNT_ROWT + FILE_COUNT_CLMT)) ) return (C);
    FILE2_read_until_newline (fp);
  }
  
  do {
    if ( fe ){
      FLOOP (j, 0, skip_clms){ FILE2_read_double (fp); if ( FILE_err&3 ) goto ROW_END; }
      x = (FILE_COUNT_INT)FILE2_read_int (fp); if ( FILE_err&3 ) goto ROW_END;
      y = (FILE_COUNT_INT)FILE2_read_int (fp); if ( FILE_err&4 ) goto ROW_END;
      FILE2_read_until_newline (fp);
    } else {
      if ( k==0 ) FLOOP (j, 0, skip_clms){ FILE2_read_double (fp); if (FILE_err&3) goto ROW_END; }
      x = t;
      y = (FILE_COUNT_INT)FILE2_read_int (fp); if (FILE_err&4 ) goto ROW_END;
      FLOOP (j, 0, int_clms){ FILE2_read_double (fp); if (FILE_err&3 ) break; }
      k++;
    }

    if ( ft ) SWAP_FILE_COUNT_INT (x, y);
    if ( y >= C.clms ){
      C.clms = y+1;
      if ( fc ) reallocx (C.clmt, C.clm_end, C.clms, 0, "file_count: clmt", goto END);
    }
    if ( x >= C.rows ){
      C.rows = x+1;
      if ( fr ) reallocx (C.rowt, C.row_end, C.rows, 0, "file_count: rowt", goto END);
    }
    if ( x < C.clm_btm || C.eles == 0 ) C.clm_btm = x;
    if ( y < C.row_btm || C.eles == 0 ) C.row_btm = y;
    if ( fc ) C.clmt[y]++;
    if ( fr ) C.rowt[x]++;
    C.eles++;

    ROW_END:;
    if ( !fe && (FILE_err&1) ){
      t++; C.rows = t;
      ENMAX (C.clm_max, k);
      ENMAX (C.clm_min, k);
      FLOOP (j, 0, int_rows) FILE2_read_until_newline (fp);
      if ( row_limit>0 && t>=row_limit ) break;
    } else if ( row_limit>0 && C.eles>=row_limit ) break;
    
  } while ( (FILE_err&2)==0 );
  END:;
  if ( C.rowt ){
    ARY_MAX (C.row_max, k, C.rowt, 0, C.rows);
    ARY_MIN (C.row_min, k, C.rowt, 0, C.rows);
  }
  if ( fe && C.clmt ){
    ARY_MAX (C.clm_max, k, C.clmt, 0, C.clms);
    ARY_MIN (C.clm_min, k, C.clmt, 0, C.clms);
  }
  if ( ERROR_MES ) mfree (C.rowt, C.clmt);
  return (C);
}


/* SLIST:very simple one-sided list */
void SLIST_init (int *l, int num, int siz){
  malloc2 (l, num+siz, "SLIST_init: l", EXIT);
  ARY_FILL (l, num, num+siz, -1);
}
void SLIST_end (int *l){ free (l); }
#define SLIST_INS(l,m,e) (l[e]=l[m],l[m]=e);

/* qsort according to "->t" */
int qsort_cmp_VECt (const void *x, const void *y){
  if ( ((VEC *)x)->t < ((VEC *)y)->t ) return (-1);
  else return ( ((VEC *)x)->t > ((VEC *)y)->t);
}
int qsort_cmp__VECt (const void *x, const void *y){
  if ( ((VEC *)x)->t > ((VEC *)y)->t ) return (-1);
  else return ( ((VEC *)x)->t < ((VEC *)y)->t);
}
void qsort_VECt (VEC *v, size_t siz, int unit){
  if ( unit == 1 || unit==-1 ) unit *= sizeof (VEC);
  if ( unit < 0 ) qsort (v, siz, -unit, qsort_cmp__VECt);
  else qsort (v, siz, unit, qsort_cmp_VECt);
} 

int qqsort_cmp_VECt (const void *x, const void *y){
  if ( QQSORT_ELEt(VEC,x) < QQSORT_ELEt(VEC,y) ) return (-1);
  else return ( QQSORT_ELEt(VEC,x) > QQSORT_ELEt(VEC,y) );
}
int qqsort_cmp__VECt (const void *x, const void *y){
  if ( QQSORT_ELEt(VEC,x) > QQSORT_ELEt(VEC,y) ) return (-1);
  else return ( QQSORT_ELEt(VEC,x) < QQSORT_ELEt(VEC,y) );
}
void qsort_perm__VECt (VEC *v, size_t siz, PERM *perm, int unit){
  if ( unit == 1 || unit==-1 ) unit *= sizeof(VEC);
  common_int=MAX(unit,-unit); common_charp=(char *)v;
  if (unit<0) qsort (perm, siz, sizeof(PERM), qqsort_cmp__VECt);
  else qsort (perm, siz, sizeof(PERM), qqsort_cmp_VECt);
}

PERM *qsort_perm_VECt (VEC *v, size_t siz, int unit){
  PERM *perm;
  malloc2 (perm, siz, "qsort_perm_VECt: perm", EXIT0);
  ARY_INIT_PERM(perm,siz);
  qsort_perm__VECt (v, siz, perm, unit);
  return(perm);
}

#ifdef STDLIB2_RADIX_SORT     // radix sort with 1M byte static memory

#define RADIX_SORT_BUCKET_SIZ 2048
/* sort of integer array with combination of radix sort and quick sort */
/* flag&1: sort in decreasing order */

// sort by lower bits
void intarray_sort_iter (unsigned int *a, size_t siz, int unit){ 
  static size_t cnt[RADIX_SORT_BUCKET_SIZ], cnt2[RADIX_SORT_BUCKET_SIZ], init_flag = 1;
  size_t k, x;
  int i, ii, j, flag=1;
  static char bbuf[1000], bbuf2[1000];
  char *aa, *aaa, *aa_end, *buf, *buf2;
  
  if ( siz<1000 ){ qsort_uint ( a, siz, unit); return; }
  if ( unit <0 ){ unit = -unit; flag = -1; }
  if ( unit == 1 ) unit = sizeof (int);
  buf = bbuf; buf2 = bbuf2;
  if ( init_flag == 1 ){
    init_flag = 0;
    ARY_FILL (cnt, 0, RADIX_SORT_BUCKET_SIZ, 0);
  }
    // count elements of each number
  for ( aa=(char*)a,aa_end=aa+siz*unit ; aa<aa_end ; aa+=unit )
      cnt[(*((unsigned int *)aa)) & (RADIX_SORT_BUCKET_SIZ-1)]++;   // difference!!

    // sum up the numbers in increasing order
  k=0; FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    cnt2[ii] = k;
    k += cnt[ii];
    cnt[ii] = k;
  }

  FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    FLOOP (x, cnt2[ii], cnt[ii]){
      aa = ((char*)a) + x*unit;
      memcpy ( buf, aa, unit);
      while (1){
        j = *((unsigned int *)buf) & (RADIX_SORT_BUCKET_SIZ-1);   // difference!!
        if ( j == ii ) break;
        aaa = ((char*)a) + cnt2[j]*unit;
//        printf ("pos[xx]=%d, cnt %d, cnt+1 %d\n", pos[xx], S->let_cnt[dep][cc], S->let_cnt[dep+1][cc]);
        memcpy ( buf2, aaa, unit);
        memcpy ( aaa, buf, unit);
        SWAP_PNT ( buf, buf2);
        cnt2[j]++;
      }
      memcpy ( aa, buf, unit);
    }
    cnt[i]=0;   // difference!!
  }
}
// sort by middle bits
void intarray_sort_iter_ ( unsigned int *a, size_t siz, int unit){ 
  static size_t cnt[RADIX_SORT_BUCKET_SIZ], cnt2[RADIX_SORT_BUCKET_SIZ], init_flag = 1;
  int i, ii, j, flag=1;
  size_t k, x;
  static char bbuf[1000], bbuf2[1000];
  char *aa, *aaa, *aa_end, *buf, *buf2;
  
  if ( siz<1000 ){ qsort_uint ( a, siz, unit); return; }
  buf = bbuf; buf2 = bbuf2;
  if ( unit <0 ){ unit = -unit; flag = -1; }
  if ( unit == 1 ) unit = sizeof (int);
  if ( init_flag == 1 ){
    init_flag = 0;
    ARY_FILL ( cnt, 0, RADIX_SORT_BUCKET_SIZ, 0);
  }
    // count elements of each number
  for ( aa=(char*)a,aa_end=aa+siz*unit ; aa<aa_end ; aa+=unit )
      cnt[((*((unsigned int *)aa))/RADIX_SORT_BUCKET_SIZ) & (RADIX_SORT_BUCKET_SIZ-1)]++;   // difference!!

    // sum up the numbers in increasing order
  k=0; FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    cnt2[ii] = k;
    k += cnt[ii];
    cnt[ii] = k;
  }

  FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    FLOOP(x, cnt2[ii], cnt[ii]){
      aa = ((char*)a) + x*unit;
      memcpy ( buf, aa, unit);
      while (1){
        j = (*((unsigned int *)buf)/RADIX_SORT_BUCKET_SIZ) & (RADIX_SORT_BUCKET_SIZ-1);   // difference!!
        if ( j == ii ) break;
        aaa = ((char*)a) + cnt2[j]*unit;
//        printf ("pos[xx]=%d, cnt %d, cnt+1 %d\n", pos[xx], S->let_cnt[dep][cc], S->let_cnt[dep+1][cc]);
        memcpy (buf2, aaa, unit);
        memcpy (aaa, buf, unit);
        SWAP_PNT (buf, buf2);
        cnt2[j]++;
      }
      memcpy (aa, buf, unit);
    }
  }
  k=0; FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    intarray_sort_iter ( (unsigned int*)(((char*)a)+unit*k), cnt[ii]-k, unit*flag);
    k = cnt[ii];
    cnt[i]=0;
  }
}

// sort by upper bits
void intarray_sort ( unsigned int *a, size_t siz, int unit){ 
  static size_t cnt[RADIX_SORT_BUCKET_SIZ], cnt2[RADIX_SORT_BUCKET_SIZ], init_flag = 1;
  int i, ii, j, flag=1;
  size_t k, x;
  static char bbuf[1000], bbuf2[1000];
  char *aa, *aaa, *aa_end, *buf, *buf2;
  
  if ( siz<1000 ){ qsort_uint ( a, siz, unit); return; }
  if ( unit <0 ){ unit = -unit; flag = -1; }
  if ( unit == 1 ) unit = sizeof (int);
  buf = bbuf; buf2 = bbuf2;
  if ( init_flag == 1){
    init_flag = 0;
    ARY_FILL (cnt, 0, RADIX_SORT_BUCKET_SIZ, 0);
  }
    // count elements of each number
  for ( aa=(char*)a,aa_end=aa+siz*unit ; aa<aa_end ; aa+=unit )
      cnt[(*((unsigned int *)aa)) / RADIX_SORT_BUCKET_SIZ / RADIX_SORT_BUCKET_SIZ]++;   // difference!!

    // sum up the numbers in increasing order
  k=0; FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    cnt2[ii] = k;
    k += cnt[ii];
    cnt[ii] = k;
  }

  FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    FLOOP (x, cnt2[ii], cnt[ii]){
      aa = ((char*)a) + x*unit;
      memcpy ( buf, aa, unit);
      while (1){
        j = *((unsigned int *)buf) / RADIX_SORT_BUCKET_SIZ / RADIX_SORT_BUCKET_SIZ;   // difference!!
        if ( j == ii ) break;
        aaa = ((char*)a) + cnt2[j]*unit;
//        printf ("pos[xx]=%d, cnt %d, cnt+1 %d\n", pos[xx], S->let_cnt[dep][cc], S->let_cnt[dep+1][cc]);
        memcpy (buf2, aaa, unit);
        memcpy (aaa, buf, unit);
        SWAP_PNT (buf, buf2);
        cnt2[j]++;
      }
      memcpy ( aa, buf, unit);
    }
  }
  k=0; FLOOP (i, 0, RADIX_SORT_BUCKET_SIZ){
    ii = flag==1? i: RADIX_SORT_BUCKET_SIZ-i-1;
    intarray_sort_iter_ ( (unsigned int*)(((char*)a)+unit*k), cnt[ii]-k, unit*flag);
    k = cnt[ii];
    cnt[i]=0;
  }

/*
  for ( i=0 ; i<siz ; i++){
    k = *((int *)(((char*)a) + i*unit));
    printf ("%d    %d,%d\n", k, k/65536, k&65535);
  }
*/
}

#endif

/* radix sort for array of structures, by their integer members 
  ranging from mm to m */
/* sort array "perm" according to (int/void*) array "a".
   if perm==NULL, allocate memory and for perm */
/* return the permutation array of the result of the sorting
   in the decreasing order if unit<0 (unimplemented!!!) */
int *radix_sort ( void *a, size_t siz, int unit, int mm, int m, int *perm){ 
  int *ll, *l, k, i, t, flag=1;
  malloc2 (l, m-mm, "radix_sort", EXIT0);
  ARY_FILL (l, 0, m-mm, -1);
  malloc2 (ll, siz, "radix_sort: ll", {free2(l);EXIT0;});
  if ( unit <0 ){ unit = -unit; flag = -1; }
  FLOOP (i, 0, (int)siz){
    k = (*((int *)(((char *)a) + unit*i ))) - mm;
    ll[i] = l[k];
    l[k] = i;
  }
  if ( perm ){
    i=0; FLOOP (k, 0, m-mm){
      while ( l[k] >= 0 ){
        t = l[k];
        l[k] = ll[t];
        ll[t] = perm[i];
        i++;
      }
    }
    memcpy (perm, ll, sizeof(int)*siz);
    free ( ll);
    free ( l);
    return ( perm);
  } else {
    i=0; FLOOP (k, 0, m-mm){
      while ( l[k] >= 0 ){
        t = l[k];
        l[k] = ll[t];
        ll[t] = i;
        i++;
      }
    }
    free (l);
    return (ll);
  }
}

/* permutate structure array *tt of unit size unit of size num, according to perm array *pp */
/* num has to be <INTHUGE/2 */
/* unit<0 means decreasing order (un-implemented!!!) */
void structure_permute (void *tt, int unit, int num, void *pp, int weight_siz){
  int i, ii, *ip, flag=1;
  char *tmp, *t=(char *)tt, *p=(char *)pp;
  if ( unit <0 ){ unit = -unit; flag = -1; }
  malloc2 (tmp, unit, "structure_permute: tmp", EXIT);
  FLOOP (i, 0, num){
    ip = (int *)(p + (sizeof(int)+weight_siz)*i + weight_siz); 
    if ( *ip< num && *ip != i ){
      ii = i;
      memcpy ( tmp, t + unit*i, unit);
      while (1) {
        if ( *ip == i ){
          memcpy ( t+unit*ii, tmp, unit);
          *ip += num;
          break;
        }
        memcpy ( t+unit*ii, t+unit*(*ip), unit);
        ii = *ip;
        *ip += num;
        ip = (int *)(p + (sizeof(int)+weight_siz)*ii + weight_siz); 
      }
    } else *ip += num;
  }
  FLOOP (i, 0, num) *(int *)(p + (sizeof(int)+weight_siz)*i + weight_siz ) -= num; 
  free (tmp);
}



#endif


/******************************************/
/*  ==== terminology for comments  ====
  range check: to check the input parameter is valid, or in the valid range.
  If a function does not have this, its comment has "no range check"
  */
   
/*  ====  rules for the name of functions and routines  ====
  init: initialization for an object, structure, etc. memory is allocated 
   if needed.
  end: compared to init, termination of structures, etc.
   free allocated memory if it exists, but not free itself.
   different from ordinary new, create, del, free.

  cpy: copy an object without allocating memory
  dup: make a duplication of an object with allocating new memory

  new: new. allocate memory for new object. also used for re-allocation from 
   the list of deleted objects
  del: delete. free memory, or insert it to the list of deleted objects

  ins : insert. insert an element (active, not deleted) to an object, possible
     at the given position.
  out : extract. extract an element, possibly specified, from an object.
        it will be not deleted.
  rm  : extract, and delete
  rmall: delete all (specified) elements of an object
  mk  : make. new+insB
  mv  : move. move the elements from an object to another,
         or change the position.

  update : update an object, possibly of specified position, to the exact,
           or to the current one.
  chg    : change the status of an object to the specified one.
  
  prv: point the previous element
  nxt: point the next element
  end: the maximum size (allocated size) of an array, etc.
  num: the (current) number of elements in an array, etc.
  kth: for the use of "first k object"
  tkth: for the use of "first k object from the end". to kth.
  rnd: random
  print: print structure and etc.
  find: search or find an specified element from the set of structures, etc.
*/

/*  ==== rules for the name of variables ====
  - use i or j for the counter in loops
  - use e for elements
  - use k for the specified rank
  - use f or flag for flagment
  - use m for maximum value or minimum value
  - use c for counters
*/


