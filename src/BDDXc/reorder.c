#include <stdio.h>
#include "bddc.h"
#include "train.h"
#include "interface.h"
#include "defs.h"
#include "reorder.h"

static train	table;
train	BDDIOpacks;
int	BDDIOfunctionlevel;

static int	samenode( a, b )
     pack	*a;
     bddp	*b;
{
  return( a->node == *b );
}


void     ClearTable(){
  TrainReset( &table, sizeof( bddp ) );
  TrainReset( &BDDIOpacks, sizeof( train ) );
}


void	FreeTable(){
  int	i;

  for( i = 0; i < TrainBound( &BDDIOpacks ); i ++ ){
    TrainFree( (train*)TrainIndex( &BDDIOpacks, i ) );
  }
  TrainFree( &table );
  TrainFree( &BDDIOpacks );
}


static void     Reserve( node )
     bddp       node;
{
  TrainLoad( &table, &node );
}


static int      IsReserved( node )
     bddp       node;
{
  if( TrainCheck( &table, &node ) == EMPTY ) return( FALSE );
  return( TRUE );
}


void	Traverse( node )
     bddp	node;
{
  bddp  left, right, nudeleft, nuderight;
  short level;
  pack	tmp;
  train	new;

  node = Strip( node );
  if( !IsReserved( node ) ){
    Reserve( node );
    level = GetLevelOf( node );
    tmp.node = node;
    if( level > 0 ){
      left = GetLeftPtrOf( node );
      nudeleft = Strip( left );
      right = GetRightPtrOf( node );
      nuderight = Strip( right );
      Traverse( left );
      Traverse( right );
      tmp.lattrib = AttributeOfEdge( left );
      tmp.left = nudeleft;
      tmp.llevel = GetLevelOf( nudeleft );
      tmp.lnumber = TrainComp( (train*)TrainIndex( &BDDIOpacks, tmp.llevel ),
			      &nudeleft, samenode );
      tmp.rattrib = AttributeOfEdge( right );
      tmp.right = nuderight;
      tmp.rlevel = GetLevelOf( nuderight );
      tmp.rnumber = TrainComp( (train*)TrainIndex( &BDDIOpacks, tmp.rlevel ),
			      &nuderight, samenode );
      while( TrainBound( &BDDIOpacks ) <= level ){
	TrainReset( &new, sizeof( pack ) );
	TrainLoad( &BDDIOpacks, &new );
      }
      TrainLoad( (train*)TrainIndex( &BDDIOpacks, level ), &tmp );
    }
    else{
      while( TrainBound( &BDDIOpacks ) <= level ){
	TrainReset( &new, sizeof( pack ) );
	TrainLoad( &BDDIOpacks, &new );
      }
      TrainLoad( (train*)TrainIndex( &BDDIOpacks, level ), &tmp );
    }
  }
}


void	TraverseFunctions( number, nodes )
     int	number;
     bddp	nodes[];
{
  int	i;
  train	new;
  pack	id;

  for( i = 0; i < number ; i ++ ){
    Traverse( nodes[ i ] );
  }
  TrainReset( &new, sizeof( pack ) );
  for( i = 0 ; i < number ; i ++ ){
    id.node = Strip( nodes[ i ] );
    id.llevel = id.rlevel = GetLevelOf( nodes[ i ] );
    id.lattrib = id.rattrib = AttributeOfEdge( nodes[ i ] );
    id.lnumber = id.rnumber = TrainComp( (train*)TrainIndex( &BDDIOpacks,
							    id.llevel ),
					&id.node, samenode );
    TrainLoad( &new, &id );
  }
  TrainLoad( &BDDIOpacks, &new );
  BDDIOfunctionlevel = TrainBound( &BDDIOpacks ) - 1;
}


int	TableMaximumBound(){
  int	i;
  int	t, max = 0;

  for( i = 0; i < TrainBound( &BDDIOpacks ) ; i ++ ){
    t = TrainBound( (train*)TrainIndex( &BDDIOpacks, i ) );
    if( t > max ){
      max = t;
    }
  }
  return( max );
}
