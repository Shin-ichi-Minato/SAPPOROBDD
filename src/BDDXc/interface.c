#include <stdio.h>
#include "bddc.h"
#include "defs.h"

int edgemode;

bddp	Strip( n_node )
     bddp	n_node;
{
  if( edgemode >= 1 ) n_node &= ~B_INV_MASK ;
  return( n_node );
}


int	AttributeOfEdge( node )
     bddp	node;
{
  int	attr;

  attr = NORMAL;
  if( edgemode >= 1 && (node & B_INV_MASK) ) attr |= NEGATIV; 
  return( attr );
}


int	SameNode( n1, n2 )
     bddp	n1, n2;
{
  return( (n1 & ~B_INV_MASK) == (n2 & ~B_INV_MASK) );
}


short	GetLevelOf( node )
     bddp	node;
{
  return( bddlevofvar(bddtop( node )) );
}


bddp	GetLeftPtrOf( node )
     bddp	node;
{
  bddp	ret;

  ret = bddat0( node, bddvaroflev(GetLevelOf( node )));
  bddfree( ret );
  return( ret );
}


bddp	GetRightPtrOf( node )
     bddp	node;
{
  bddp	ret;
  
  ret = bddat1( node, bddvaroflev(GetLevelOf( node )));
  bddfree( ret );
  return( ret );
}


int	NameOfLeaf( leaf )
     bddp	leaf;
{
  if( leaf == bddfalse ){
    return( 0 );
  }
  else if( leaf == bddtrue ){
    return( 1 );
  }
  else if( leaf == bddnull ){
    return( -1 );
  }
  else{
    fprintf( stderr, "Something is wrong. (%d)\n" , leaf);
  }
}
