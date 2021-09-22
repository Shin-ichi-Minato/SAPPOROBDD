/****************************************
*  Generating BDDs for N-Queens problem *
* (c) Shin-ichi Minato (2011/10/15)     *
****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bddc.h"

#ifdef B_64
#  define B_STRTOI strtoll
#  define B_ITOSTR(n, s) sprintf(s, "%lld", n)
#else
#  define B_STRTOI strtol
#  define B_ITOSTR(n, s) sprintf(s, "%d", n)
#endif

int main(int argc, char *argv[])
{
  int q, n, i, j, x, y;
  char s[256];
  bddp** X;
  bddp bddlimit, F, G, F0, F1, X0, G0, G01, G11;

  /**** Read parameters ****/
  if(argc < 2 || argc > 3)
  {
    printf("bddqueen <problem_size> [<bdd_node_limit>]\n");
    return 1;
  }

  q = atoi(argv[1]);
  if(argc == 3) bddlimit = B_STRTOI(argv[2], NULL, 10);
  else bddlimit = bddnull;

  printf("problem_size:  %d\n", q);
  B_ITOSTR(bddlimit, s);
  printf("bdd_node_limit:%s\n", s);

  /**** Initialize BDD package ****/
  if(bddinit(64, bddlimit))
  {
    printf("memory allocation failed.\n");
    return 1;
  }

  /**** Generate BDDs for respective queens ****/
  X = malloc(sizeof(bddp*) * q);
  for(i=0; i<q; i++)
  {
    X[i] = malloc(sizeof(bddp) * q);
    for(j=0; j<q; j++) X[i][j] = bddprime(bddnewvar());
  }
    
  /**** Repeat BDD generation for 3 times ****/
  for(n=0; n<3; n++)
  {
    printf("\n--------[%d]--------\n", n+1);
    F = bddtrue;

    /**** Generating BDDs for row constraints ****/
    for(i=0; i<q; i++)
    {
      F0 = bddtrue;
      F1 = bddfalse;
      for(j=0; j<q; j++)
      {
        X0 = bddnot(X[i][j]);
        G0 = bddand(F0, X0);
        G01 = bddand(F0, X[i][j]);
        G11 = bddand(F1, X0);
        bddfree(X0);
        bddfree(F0);
        bddfree(F1);
        F0 = G0;
        F1 = bddor(G01, G11);
        bddfree(G01);
        bddfree(G11);
      }
      bddfree(F0);
      G = bddand(F, F1);
      bddfree(F);
      bddfree(F1);
      F = G;
      printf(".");
      fflush(stdout);
    }
    for(i=0; i<q-3; i++) putchar(' ');
    B_ITOSTR(bddsize(F), s);
    printf("size:%s ", s);
    B_ITOSTR(bddused(), s);
    printf("used:%s\n", s);
  
    /**** Generating BDDs for column constraints ****/
    for(i=0; i<q; i++)
    {
      F0 = bddtrue;
      F1 = bddfalse;
      for(j=0; j<q; j++)
      {
        X0 = bddnot(X[j][i]);
        G0 = bddand(F0, X0);
        G01 = bddand(F0, X[j][i]);
        G11 = bddand(F1, X0);
        bddfree(X0);
        bddfree(F0);
        bddfree(F1);
        F0 = G0;
        F1 = bddor(G01, G11);
        bddfree(G01);
        bddfree(G11);
      }
      bddfree(F0);
      G = bddand(F, F1);
      bddfree(F);
      bddfree(F1);
      F = G;
      printf(".");
      fflush(stdout);
    }
    for(i=0; i<q-3; i++) putchar(' ');
    B_ITOSTR(bddsize(F), s);
    printf("size:%s ", s);
    B_ITOSTR(bddused(), s);
    printf("used:%s\n", s);
  
    /**** Generating BDDs for upward diagonal constraints ****/
    for(i=0; i<q*2-3; i++)
    {
      F0 = bddtrue;
      F1 = bddfalse;
      for(j=0; j<q; j++)
      {
        x = i - (q -2) + j;
        y = q - 1 - j; 
        if(x < 0 || x >= q) continue;
        if(y < 0 || y >= q) continue;
        X0 = bddnot(X[x][y]);
        G0 = bddand(F0, X0);
        G01 = bddand(F0, X[x][y]);
        G11 = bddand(F1, X0);
        bddfree(X0);
        bddfree(F0);
        bddfree(F1);
        F0 = G0;
        F1 = bddor(G01, G11);
        bddfree(G01);
        bddfree(G11);
      }
      G = bddor(F0, F1);
      bddfree(F0);
      bddfree(F1);
      F1 = bddand(F, G);
      bddfree(F);
      bddfree(G);
      F = F1;
      printf(".");
      fflush(stdout);
    }
    B_ITOSTR(bddsize(F), s);
    printf("size:%s ", s);
    B_ITOSTR(bddused(), s);
    printf("used:%s\n", s);
  
    /**** Generating BDDs for downward diagonal constraints ****/
    for(i=0; i<q*2-3; i++)
    {
      F0 = bddtrue;
      F1 = bddfalse;
      for(j=0; j<q; j++)
      {
        x = i - (q -2) + j;
        y = j; 
        if(x < 0 || x >= q) continue;
        if(y < 0 || y >= q) continue;
        bddp X0 = bddnot(X[x][y]);
        bddp G0 = bddand(F0, X0);
        bddp G01 = bddand(F0, X[x][y]);
        bddp G11 = bddand(F1, X0);
        bddfree(X0);
        bddfree(F0);
        bddfree(F1);
        F0 = G0;
        F1 = bddor(G01, G11);
        bddfree(G01);
        bddfree(G11);
      }
      G = bddor(F0, F1);
      bddfree(F0);
      bddfree(F1);
      F1 = bddand(F, G);
      bddfree(F);
      bddfree(G);
      F = F1;
      printf(".");
      fflush(stdout);
    }
    B_ITOSTR(bddsize(F), s);
    printf("size:%s ", s);
    B_ITOSTR(bddused(), s);
    printf("used:%s\n", s);
  
    /**** Check memory overflow ****/
    if(F == bddnull) printf("Memory overflow.\n");

    /**** Crear the final BDD ****/
    bddfree(F);
  }

  return 0;
}
