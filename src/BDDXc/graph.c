#include <stdio.h>
#include "bddc.h"
#include "defs.h"
#include "train.h"
#include "reorder.h"

#define NODEBLUSH	2
#define DEFAULTRADIX	20
#define SPOTRADIX	3
#define CHECKCOUNT      20

extern train	BDDIOpacks;
extern int	BDDIOfunctionlevel;
extern int	BDDIOwidth, BDDIOheight;

static unsigned int	radix = DEFAULTRADIX;

static int	top, toe;
static int	fore, back;


void	SetRadix( r )
     int	r;
{
  if( r > DEFAULTRADIX ){
    radix = DEFAULTRADIX;
  }
  else{
    radix = r;
  }
}


int	GetRadix(){
  return( radix );
}


void	PutNode( level, number )
     short	level;
     int	number;
{
  int		x, y;
  
  Center( level, number, &x, &y );
  if( level == BDDIOfunctionlevel ){
    Circle( x, y, radix, fore, NODEBLUSH );
    DrawBoldInt( number + 1, x, y );
  }
  else if( level > 0 ){
    Circle( x, y, radix, fore, NODEBLUSH );
    if( radix > MeanFontWidth() ){
      DrawInt( level, x, y );
    }
  }
  else{
    Square( x, y, radix, fore, NODEBLUSH );
    DrawInt( NameOfLeaf( *(bddp*)TrainIndex( (train*)TrainIndex( &BDDIOpacks,
								level ),
					    number) ),
	    x, y );
  }
}


void	LeftEdge( flevel, fnumber, tlevel, tnumber )
     short	flevel, tlevel;
     int	fnumber, tnumber;
{
  int	x0, y0, xf, yf, xp, yp, xt, yt;

  if( flevel > 0 ){
    Center( flevel, fnumber, &x0, &y0 );
    LeftLeg( flevel, fnumber, &xf, &yf );
    LeftFoot( flevel, fnumber, &xp, &yp );
    Head( tlevel, tnumber, &xt, &yt );
    if( xf > xt ){
      RightShoulder( tlevel, tnumber, &xt, &yt );
    }
    else if( xf < xt ){
      LeftShoulder( tlevel, tnumber, &xt, &yt );
    }
    Curve( x0, y0, xf, yf, xp, yp, xt, yt, ( flevel - tlevel != 1 ) );
  }
}


void	RightEdge( flevel, fnumber, tlevel, tnumber )
     short      flevel, tlevel;
     int        fnumber, tnumber;
{
  int	x0, y0, xf, yf, xp, yp, xt, yt;

  if( flevel > 0 ){
    Center( flevel, fnumber, &x0, &y0 );
    RightLeg( flevel, fnumber, &xf, &yf );
    RightFoot( flevel, fnumber, &xp, &yp );
    Head( tlevel, tnumber, &xt, &yt );
    if( xf > xt ){
      RightShoulder( tlevel, tnumber, &xt, &yt );
    }
    else if( xf < xt ){
      LeftShoulder( tlevel, tnumber, &xt, &yt );
    }
    Curve( x0, y0, xf, yf, xp, yp, xt, yt, ( flevel - tlevel != 1 ) );
  }
}


void	TailEdge( flevel, fnumber, tlevel, tnumber )
     short      flevel, tlevel;
     int        fnumber, tnumber;
{
  int	x1, x2, y1, y2;

  if( flevel > 0 ){
    Hip( flevel, fnumber, &x1, &y1 );
    Head( tlevel, tnumber, &x2, &y2 );
    if( x1 > x2 ){
      RightShoulder( tlevel, tnumber, &x2, &y2 );
    }
    else if( x1 < x2 ){
      LeftShoulder( tlevel, tnumber, &x2, &y2 );
    }
    Line( x1, y1, x2, y2 );
  }
}


static void	NegativPochi( level, number )
     short	level;
     int	number;
{
  int	x, y;

  if( level != BDDIOfunctionlevel ){
    RightLeg( level, number, &x, &y );
  }
  else{
    Hip( level, number, &x, &y );
  }
/*  Circle( x, y, SPOTRADIX / 2, back, SPOTRADIX / 2 + 2 );*/
  Circle( x, y, SPOTRADIX, fore, NODEBLUSH );
}


static void	LeftNegativPochi( level, number )
     short	level;
     int	number;
{
  int	x, y;

  if( level != BDDIOfunctionlevel ){
    LeftLeg( level, number, &x, &y );
  }
  else{
    Hip( level, number, &x, &y );
  }
/*  Circle( x, y, SPOTRADIX / 2, back, SPOTRADIX / 2 + 2 );*/
  Circle( x, y, SPOTRADIX, fore, NODEBLUSH );
}


void	ExPochi( level, number )
     short	level;
     int	number;
{
  int	x, y;

  if( level != BDDIOfunctionlevel ){
    RightLeg( level, number, &x, &y );
  }
  else{
    Hip( level, number, &x, &y );
  }
  Cross( x, y, SPOTRADIX * 2, NODEBLUSH );
}

void	LeftExPochi( level, number )
     short	level;
     int	number;
{
  int	x, y;

  if( level != BDDIOfunctionlevel ){
    LeftLeg( level, number, &x, &y );
  }
  else{
    Hip( level, number, &x, &y );
  }
  Cross( x, y, SPOTRADIX * 2, NODEBLUSH );
}


static int	HaveTheAttribute( atr, name )
     int	atr, name;
{
  return( ( atr & name ) != 0 );
}


static void	DrawNodes(){
  int	i, j;
  int   cnt;
  train	*p;
  pack	*pk;
  
  cnt = 0;
  for( i = toe; i <= top ; i ++ ){
    p = ( train *)TrainIndex( &BDDIOpacks, i );
    for( j = 0; j < TrainBound( p ) ; j ++ ){
      pk = ( pack *)TrainIndex( p, j );
      if( cnt == 0 ){
	cnt = CHECKCOUNT;
	if( Interrupt() ){
	  goto loopabort;
	}
      }
      else{
	cnt --;
      }
      PutNode( i, j );
      if( i == BDDIOfunctionlevel ){
	TailEdge( i, j, pk->llevel, pk->lnumber );
	if( HaveTheAttribute( pk->lattrib, NEGATIV ) ) NegativPochi( i, j );
	if( HaveTheAttribute( pk->lattrib, EXCHANGE ) ) ExPochi( i, j );
      }
      else if( i > 0 ){
	if( ( AttributeOfEdge(pk->node) & EXCHANGE ) != 0 ){
	  RightEdge( i, j, pk->llevel, pk->lnumber );
	  if( HaveTheAttribute( pk->lattrib, NEGATIV ) ) NegativPochi( i, j );
	  if( HaveTheAttribute( pk->lattrib, EXCHANGE ) ) ExPochi( i, j );
	  LeftEdge( i, j, pk->rlevel, pk->rnumber );
	  if( HaveTheAttribute( pk->rattrib, NEGATIV ) ) LeftNegativPochi( i, j );
	  if( HaveTheAttribute( pk->rattrib, EXCHANGE ) ) LeftExPochi( i, j );
	}
	else{
	  LeftEdge( i, j, pk->llevel, pk->lnumber );
	  if( HaveTheAttribute( pk->lattrib, NEGATIV ) ) LeftNegativPochi( i, j );
	  if( HaveTheAttribute( pk->lattrib, EXCHANGE ) ) LeftExPochi( i, j );
	  RightEdge( i, j, pk->rlevel, pk->rnumber );
	  if( HaveTheAttribute( pk->rattrib, NEGATIV ) ) NegativPochi( i, j );
	  if( HaveTheAttribute( pk->rattrib, EXCHANGE ) ) ExPochi( i, j );
	}
      }
    }
  }
 loopabort:;
}


void	Show(){
  ResetWindowSize();
  DrawNodes();
}


void	Draw( server, fontname )
     char	*server;
     char	*fontname;
{
  top = TrainBound( &BDDIOpacks ) - 1;
  toe = 0;
  LocationSetMaximumSize( TableMaximumBound(), top );
  if( WindowOpen( server, fontname ) != FALSE ){
    QueryColor( &fore, &back );
    Wait();
    WindowClose();
  }
}


void	BDDgraph0( server, fontname, number, bddps )
     char	*server, *fontname;
     int	number;
     bddp	bddps[];
{
  edgemode = 0;
  ClearTable();
  TraverseFunctions( number, bddps );
  Draw( server, fontname );
  FreeTable();
}

void	BDDgraph1( server, fontname, number, bddps )
     char	*server, *fontname;
     int	number;
     bddp	bddps[];
{
  edgemode = 1;
  ClearTable();
  TraverseFunctions( number, bddps );
  Draw( server, fontname );
  FreeTable();
}

void	BDDgraph2( server, fontname, number, bddps )
     char	*server, *fontname;
     int	number;
     bddp	bddps[];
{
  edgemode = 2;
  ClearTable();
  TraverseFunctions( number, bddps );
  Draw( server, fontname );
  FreeTable();
}

void	SetWindowSize( x, y )
     int	x, y;
{
  BDDIOwidth = x;
  BDDIOheight = y;
}

void bddgraph0(f)
bddp f;
{
	if(f != bddnull) BDDgraph0(0, 0, 1, &f);
}


void bddvgraph0(ptr, lim)
bddp *ptr;
int lim;
{
        int n;

	n = 0;
	while(n < lim)
	{
	  if(ptr[n] == bddnull) break;
	  n++;
	}
	BDDgraph0(0, 0, n, ptr);
}

void bddgraph(f)
bddp f;
{
	if(f != bddnull) BDDgraph1(0, 0, 1, &f);
}


void bddvgraph(ptr, lim)
bddp *ptr;
int lim;
{
        int n;

	n = 0;
	while(n < lim)
	{
	  if(ptr[n] == bddnull) break;
	  n++;
	}
	BDDgraph1(0, 0, n, ptr);
}


