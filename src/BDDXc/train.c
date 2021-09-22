#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "train.h"
#include "defs.h"


void	TrainReset( root, size )
     train	*root;
     int	size;
{
  container	*p;
  
  root->size = size;
  p = ( container *)malloc( sizeof( container ) );
  root->head = p;
  root->tail = p;
  root->bound = 0;
  p->rest = N;
  p->next = NULL;
  p->nodes = ( dummy *)malloc( N * root->size );
  p->tail = p->nodes;
}


void	TrainFree( root )
     train      *root;
{
  container     *t, *tv;

  t = root->head;
  do{
    tv = t;
    t = t->next;
    free( tv->nodes );
    free( tv );
  }while( t != NULL );
  root->head = NULL;
  root->tail = NULL;
  root->size = 0;
}


static void	AllocContainer( root )
     train	*root;
{
  container	*p;

  p = ( container *)malloc( sizeof( container ) );
  root->tail->next = p;
  root->tail = p;
  p->rest = N;
  p->next = NULL;
  p->nodes = ( dummy * )malloc( N * root->size );
  p->tail = p->nodes;
}


void	TrainLoad( root, node )
     train	*root;
     dummy	*node;
{
  if( root->tail->rest == 0 ) AllocContainer( root );
  bcopy( node, root->tail->tail, root->size );
  root->tail->tail += root->size;
  root->tail->rest --;
  root->bound ++;
}


int	TrainCheck( root, node )
     train	*root;
     dummy	*node;
{
  container	*t;
  int		i, x;

  t = root->head;
  x = 0;
  do{
    for( i = 0; i < N - ( t->rest ); i++ ){
      if( bcmp( t->nodes + i * root->size , node, root->size ) == 0 )
	return( x + i );
    }
    t = t->next;
    x += N;
  }while( t != NULL );
  return( EMPTY );
}


int	TrainComp( root, node, func )
     train	*root;
     dummy	*node;
     int	(*func)();
{
  container	*t;
  int		i, x;

  t = root->head;
  x = 0;
  do{
    for( i = 0; i < N - ( t->rest ); i++ ){
      if( (*func)( t->nodes + i * root->size , node ) )
	return( x + i );
    }
    t = t->next;
    x += N;
  }while( t != NULL );
  return( EMPTY );
}


dummy	*TrainIndex( root, x )
     train	*root;
     int	x;
{
  container	*t;

  if( root->bound <= x ){
    fprintf( stderr, "ChuDoon\n" );
    exit( 1 );
  }
  t = root->head;
  do{
    if( N - ( t->rest ) > x ) return( t->nodes + x * root->size );
    t = t->next;
    x -= N;
  }while( t != NULL );
  return( NULL );
}


int	TrainBound( root )
     train	*root;
{
  return( root->bound );
}

