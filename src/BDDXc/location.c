#include "defs.h"
#include "train.h"

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


int	Head( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy - GetRadix() );
}


int	Top( level, number, x, y )
     short	level;
     int	number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy - 2 * GetRadix() );
}


int	LeftHand( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() );
  *y = ( cy - 2 * GetRadix() );
}


int	RightHand( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() );
  *y = ( cy - 2 * GetRadix() );
}


int	LeftLeg( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() / 4 * 3 );
  *y = ( cy + GetRadix() / 4 * 3 );
}

int	RightLeg( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() / 4 * 3 );
  *y = ( cy + GetRadix() / 4 * 3 );
}

int	LeftFoot( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx - GetRadix() * 2  );
  *y = ( cy + GetRadix() * 2  );
}

int	RightFoot( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx + GetRadix() * 2  );
  *y = ( cy + GetRadix() * 2  );
}

int	Hip( level, number, x, y )
     short      level;
     int        number;
     int	*x, *y;
{
  int	cx, cy;

  Center( level, number, &cx, &cy );
  *x = ( cx );
  *y = ( cy + GetRadix() );
}

int	RightShoulder( level, number, x, y )
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

int	LeftShoulder( level, number, x, y )
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

