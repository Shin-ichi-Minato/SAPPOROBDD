#include "defs.h"
#include "train.h"

extern void SetRadix();
extern int GetRadix();

extern train	BDDIOpacks;
static unsigned int	width, height;

static int	top, span;
static int	xinterval, yinterval;


void	LocationResetLocation( w, h )
     unsigned int	w, h;
{
  int	r, rx, ry;
  
  width = w;
  height = h;
  xinterval = width / ( span + 1 );
  yinterval = height / ( top + 2 );
  rx = xinterval / 3;
  ry = yinterval / 3;
  if( rx > ry ){
    r = ry;
  }
  else{
    r = rx;
  }
  SetRadix( r );
}


void	LocationSetMaximumSize( x, y )
     int	x, y;
{
  span = x;
  top = y;
  LocationResetLocation( width, height );
}


void	Center( level, number, x, y )
     short	level;
     int	number;
     int	*x, *y;
{
  int	size;

  size = TrainBound( (train*)TrainIndex( &BDDIOpacks, level ) );
  *x = (
	(int)width
	+ ( 2 * number - ( size - 1 ) )
	  * (int)width / ( size + 1 )
	) / 2;
  *y = yinterval * ( top - level + 1 );
}


void	Head( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy - GetRadix() );
}


void	Top( level, number, x, y )
     short	level;
     int	number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy - 2 * GetRadix() );
}


void	LeftHand( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() );
  *y = ( cy - 2 * GetRadix() );
}


void	RightHand( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() );
  *y = ( cy - 2 * GetRadix() );
}


void	LeftLeg( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() / 4 * 3 );
  *y = ( cy + GetRadix() / 4 * 3 );
}

void	RightLeg( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() / 4 * 3 );
  *y = ( cy + GetRadix() / 4 * 3 );
}

void	LeftFoot( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() * 2  );
  *y = ( cy + GetRadix() * 2  );
}

void	RightFoot( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() * 2  );
  *y = ( cy + GetRadix() * 2  );
}

void	Hip( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy + GetRadix() );
}

void	RightShoulder( level, number, x, y )
     short	level;
     int	number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() * 3 / 4 );
  if( level != 0 )
    *y = ( cy - GetRadix() * 3 / 4 );
  else
    *y = ( cy - GetRadix() );
}

void	LeftShoulder( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() * 3 / 4 );
  if( level != 0 )
    *y = ( cy - GetRadix() * 3 / 4 );
  else
    *y = ( cy - GetRadix() );
}

