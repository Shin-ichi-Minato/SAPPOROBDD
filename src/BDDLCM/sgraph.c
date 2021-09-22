/*  graph library by array list
            12/Feb/2002    by Takeaki Uno
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users.
   For the commercial use, please make a contact to Takeaki Uno. */

#ifndef _sgraph_c_
#define _sgraph_c_

#include"sgraph.h"
#include"vec.c"

SGRAPH INIT_SGRAPH = {TYPE_SGRAPH,INIT_SETFAMILY_,INIT_SETFAMILY_,INIT_SETFAMILY_,0,0,NULL,NULL};

/*  initialization  */
void SGRAPH_alloc (SGRAPH *G, QUEUE_ID nodes, size_t edge_num, size_t arc_num){
  if ( edge_num > 0 ){
    SETFAMILY_alloc (&G->edge, nodes, NULL, nodes, edge_num);
    if ( G->flag&LOAD_EDGEW && (!ERROR_MES) ) SETFAMILY_alloc_weight (&G->edge);
  }
  if ( arc_num > 0 ){
    SETFAMILY_alloc (&G->in, nodes, NULL, nodes, arc_num);
    SETFAMILY_alloc (&G->out, nodes, NULL, nodes, arc_num);
    if ( G->flag&LOAD_EDGEW && (!ERROR_MES) ){
      SETFAMILY_alloc_weight (&G->in);
      SETFAMILY_alloc_weight (&G->out);
    }
  }
  if (G->flag&LOAD_NODEW) calloc2 (G->node_w, nodes, "SGRAPH_alloc: node_w", G->node_w=0);
  if ( ERROR_MES ){ SGRAPH_end (G); EXIT; }
}

/* copy graph G to graph G2. Underconstruction */
//void SGRAPH_cpy (SGRAPH *G2, SGRAPH *G){}

/* free graph object  */
void SGRAPH_end (SGRAPH *G){
  SETFAMILY_end (&G->edge);
  SETFAMILY_end (&G->in);
  SETFAMILY_end (&G->out);
  mfree (G->wbuf, G->perm);
  *G = INIT_SGRAPH;
}


/*  make an edge between u and v.
   If they are already connected, it will be a multiple edge */
void SGRAPH_edge_mk (SGRAPH *G, QUEUE_INT u, QUEUE_INT v, WEIGHT w){
  if ( G->edge.w ){
    G->edge.w[u][G->edge.v[u].t] = w;
    G->edge.w[v][G->edge.v[v].t] = w;
  }
  ARY_INS (G->edge.v[u], v);
  ARY_INS (G->edge.v[v], u);
  G->edge.eles += 2;
}

/*  make an arc between u and v.
   If they are already connected, it will be a multiple arc */
void SGRAPH_arc_mk (SGRAPH *G, QUEUE_INT u, QUEUE_INT v, WEIGHT w){
  if ( G->out.w ) G->out.w[u][G->out.v[u].t] = w;
  if ( G->in.w ) G->in.w[v][G->in.v[v].t] = w;
  ARY_INS (G->out.v[u], v);
  ARY_INS (G->in.v[v], u);
  G->in.eles++;
  G->out.eles++;
}

/* Delete the edge connecting u and v. If edge (u,v) does not exist, nothing will occur. */
void SGRAPH_edge_rm_iter (SETFAMILY *M, QUEUE_INT u, QUEUE_INT v){
  QUEUE_INT i;
  if ( (i = (QUEUE_INT)QUEUE_ele (&M->v[u], v)) >= 0 ){
    QUEUE_rm (&M->v[u], i);
    if ( M->w ) M->w[u][i] = M->w[u][M->v[u].t];
    M->eles--;
  }
}

/* Delete the edge connecting u and v. If edge (u,v) does not exist, nothing will occur. */
void SGRAPH_edge_rm (SGRAPH *G, QUEUE_INT u, QUEUE_INT v){
  SGRAPH_edge_rm_iter (&G->edge, u, v);
  SGRAPH_edge_rm_iter (&G->edge, v, u);
}

/* Delete the arc connecting u and v. If arc (u,v) does not exist, nothing will occur. */
void SGRAPH_arc_rm (SGRAPH *G, QUEUE_INT u, QUEUE_INT v){
  SGRAPH_edge_rm_iter (&G->out, u, v);
  SGRAPH_edge_rm_iter (&G->in, v, u);
}

/*  print graph by numbers  */
void SGRAPH_print (SGRAPH *G){
  VEC_ID i;
  QUEUE_ID j;
  QUEUE_INT e;
  
  printf ("#node "VEC_IDF" ,#edge %zd ,#arc %zd\n", SGRAPH_NODE_NUM(*G), G->edge.eles, G->in.eles);
  FLOOP (i, 0, SGRAPH_NODE_NUM(*G)){
    printf ("NODE "VEC_IDF" ", i);
    if ( G->node_w ){ putchar ('('); print_WEIGHT (G->node_w[i]); putchar (')'); }
    printf (" >>\n");
    if ( G->edge.v && G->edge.v[i].t ){
      printf ("    edge      : ");
      ARY_FLOOP (G->edge.v[i], j, e){
        printf (VEC_IDF, e);
        if ( G->edge.w ){ putchar ('('); print_WEIGHT (G->edge.w[i][j]); putchar (')'); }
        putchar (',');
      }
      putchar ('\n');
    }
    if ( G->in.w ){
      if ( G->in.v[i].t ){
        printf ("    in-arc      : ");
        ARY_FLOOP (G->in.v[i], j, e){
          printf (VEC_IDF, e);
          if ( G->in.w ){ putchar ('('); print_WEIGHT (G->in.w[i][j]); putchar (')'); }
          putchar (',');
        }
        putchar ('\n');
      }
    }
    if ( G->out.w ){
      if ( G->out.v[i].t ){
        printf ("    out-arc      : ");
        ARY_FLOOP (G->out.v[i], j, e){
          printf (VEC_IDF, e);
          if ( G->out.w ){ putchar ('('); print_WEIGHT (G->out.w[i][j]); putchar (')');}
          putchar (',');
        }
        putchar ('\n');
      }
    }
  }
}

/* Output a graph to file
  Vertices, edges, arcs less than node_num, edge_num, arc_num are written to the file. Input parameters are
  (graph) (file name) (flag)
  SGRAPH_READ_NODEW 512 // read node weight
  SGRAPH_READ_EDGEW 1024 // read edge weight
*/
/*
  format of file:(including notifications to make input file)
   
  the ith row corresponds to node i-1, and
    ID list of nodes adjacent to i, and having ID > i, for undirected graph
    ID list of nodes adjacent to i by out-going arc of i, for directed graph
   Separator is ",", but graph load routine accepts any letter for 
    separator but not a number.
   If the graph has both edges and arcs, write them in two lines separately,
    so a node then uses two lines, and #nodes = #lines/2.
  
    ==  Notifications to make input file ==
   Notice that if 0th line has node 2, and the 2nd line has 0, then there
    will be multiple edge (0,2) and (2,0).
   The read routine does not make error with multiple edges, it is allowed.

   The ID of nodes begin from 0. After reading graph, node_num is set to
    node_end.

   Input file example, without weights, E={(0,1),(0,2),(1,1),(1,3),(2,3)}
===========
   1,2
   1 3
   3
   
   [EOF]
=========
   Nodes are 0,1, and 2, both edges and arcs exist, with node/edge/arc weights)
   5000,1,30
   0,50,1,20,
   100,1,3
   2,20
   200
   
   [EOF]
=======
   where node weights are 5000, 100, 200, and edges and their weights are
    (0,1),30,   (1,1),3
    arcs and their weights are (0,0),50,   (0,1), 20,   (1,2), 20

    In the case of bipartite graph, write the adjacent-node lists only for 
     the node in node set one.
     
    
*/

/* graph load routine. Allocate memory as much as the size of input file.
   parameters are, 
   (graph) (file name) 
 LOAD_EDGE // read undirected edge from file
 LOAD_ARC // read directed arc from file
 LOAD_BIPARTITE // load bipartite graph
 LOAD_NODEW // read node weight
 LOAD_EDGEW // read edge weight
*/
/* In the bipartite case, even if the IDs of node set 2 begin from 0, i.e.,
   overlaps with node 1, the routine automatically correct them. */
/* Directed bipartite graph, all arcs are considered to be from node set 1
 to node set 2. If both directions exist, read as a general graph, and set
  node1_num later in some way. */
/* The routine compares the maximum node index and #lines, and set #node
  to the larger one. However, if node weight exists, weights will be included 
  in the candidates of maximum index, thus in this case we fix #node := #lines.
  In the case of bipartite graph, the routine compares, but the weights of 
   non-existing lines will be -1. */

/* make the opposite direction edge, for each edge; buffers have to be already doubly allocated */
void SGRAPH_load_delivery (SGRAPH *G, SETFAMILY *OO, SETFAMILY *MM, QUEUE_ID *c){
  VEC_ID i;
  QUEUE_ID j;
  QUEUE_INT e;
  FLOOP (i, 0, MM->t) c[i] = MM->v[i].t;
  FLOOP (i, 0, MM->t){
    FLOOP (j, 0, c[i]){
      e = MM->v[i].v[j];
      if ( OO->w ) OO->w[e][OO->v[e].t] = MM->w[i][j];
      ARY_INS (OO->v[e], i);
    }
  }
}

/* make the opposite direction edge, for each edge; buffers have to be already doubly allocated */
void SGRAPH_mk_opposite_edge (SGRAPH *G, QUEUE_ID *c){
  VEC_ID i;
  size_t j, jj;
  j = G->edge.eles;  // shift the arrays to insert edges of opposite directions
  BLOOP (i, G->edge.t, 0){
    j -= G->edge.v[i].t+c[i];
    jj = G->edge.v[i].t+1;
    do {
      jj--;
      G->edge.buf[j+i+jj] = G->edge.v[i].v[jj];
    } while ( jj>0 );
    G->edge.v[i].end += c[i];
    G->edge.v[i].v = &G->edge.buf[j+i];
    if ( G->edge.w ){
      memcpy ( &G->edge.buf[j], G->edge.w[i], sizeof(WEIGHT)*G->edge.v[i].t );
      G->edge.w[i] = &G->edge.wbuf[j];
    }
  }
}

/* load edges/arcs (determined by G->flag) from file */
void SGRAPH_load (SGRAPH *G, char *fname, char *wfname){
  VEC_ID i;
  QUEUE_ID *c;
  SETFAMILY *F1, *F2;

  if ( G->flag&LOAD_EDGE ){ F1 = F2 = &G->edge; G->edge.flag |= LOAD_DBLBUF; }
  else { F1 = &G->in; F2 = &G->out; }
  SETFAMILY_load (F1, fname, wfname);
    // adjact so that #rows and #colums are the same
  if ( !(G->flag&LOAD_BIPARTITE)){
    if ( F1->clms < F1->t ){
      F1->clms = F1->t;
      FLOOP (i, 0, F1->t) F1->v[i].v[F1->v[i].t] = F1->t; // re-set endmark
    } else if ( F1->clms > F1->t ){
      reallocx_ (F1->v, F1->t, F1->clms, INIT_QUEUE, "SGRAPH_load: v", EXIT);
      FLOOP (i, F1->t, F1->clms){
        F1->v[i].v = F1->v[F1->t -1].v +F1->v[F1->t -1].t +1 +(i -(F1->t-1));
        F1->v[i].v[0] = F1->clms;
      } // re-set endmark
      F1->t = F1->clms;
    }
  }

  calloc2 (c, F1->t, "SGRAPH_load: c", EXIT);
  QUEUE_delivery (NULL, c, NULL, F1->v, NULL, F1->t, F1->t);
// SETFAMILY_print (stdout, F1);

  if ( F1 != F2 ) SETFAMILY_alloc (F2, F1->t, c, F1->t, 0);
  else {
    G->edge.eles *= 2;  G->edge.ele_end *= 2;
    SGRAPH_mk_opposite_edge (G, c); // shift the arrays to insert edges of opposite directions
  }

  SGRAPH_load_delivery (G, F2, F1, c);
  free (c);
  F2->clms = F2->t; FLOOP (i, 0, F2->t) F2->v[i].v[F2->v[i].t] = F2->t; // re-set endmark

  F1->flag |= G->flag; SETFAMILY_sort (F1);
  if ( F1 != F2 ){ F2->flag |= G->flag; SETFAMILY_sort (F2); }
}

/* replace node i by perm[i] */
void SGRAPH_replace_index (SGRAPH *G, PERM *perm, PERM *invperm){
  QUEUE_INT *x;
  VEC_ID i;
  QUEUE Q;
  WEIGHT *w, ww;
  
  FLOOP (i, 0, G->edge.t)
  if ( G->edge.v ){
    MQUE_FLOOP (G->edge.v[i], x) *x = perm[*x];
    ARY_INVPERMUTE (G->edge.v, invperm, Q, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  }
  if ( G->in.v ){
    MQUE_FLOOP (G->in.v[i], x) *x = perm[*x];
    ARY_INVPERMUTE (G->in.v, invperm, Q, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  }
  if ( G->out.v ){
    MQUE_FLOOP (G->out.v[i], x) *x = perm[*x];
    ARY_INVPERMUTE (G->out.v, invperm, Q, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  }
  if ( G->edge.w ) ARY_INVPERMUTE (G->edge.w, invperm, w, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  if ( G->in.w ) ARY_INVPERMUTE (G->in.w, invperm, w, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  if ( G->out.w ) ARY_INVPERMUTE (G->out.w, invperm, w, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  if ( G->node_w ) ARY_INVPERMUTE (G->node_w, invperm, ww, G->edge.t, "SGRAPH_repl_ind:", EXIT);
  G->perm = perm;
}

/* sort the nodes by Q->t, increasing if flag=1, decreasing if flag=-1 */
void SGRAPH_perm_node (SGRAPH *G, PERM *tmp){
  VEC_ID c1=0, c2=G->node1_num, i;
  PERM *perm;
  malloc2 (perm, G->edge.t, "SGRAPH_perm_node", {free(tmp);EXIT;});
  FLOOP (i, 0, G->edge.t)
      if ( tmp[i]<G->node1_num ) perm[tmp[i]] = c1++; else perm[tmp[i]] = c2++;
  ARY_INV_PERM_ (tmp, perm, G->edge.t);
  SGRAPH_replace_index (G, perm, tmp);
  free2 (tmp);
}

#endif
