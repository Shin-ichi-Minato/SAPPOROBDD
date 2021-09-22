#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "defs.h"

#define EDGEBLUSH	1

extern Display	*disp;
extern Window	window;
extern XFontStruct	*fontinfo;
extern GC	wingc;
extern int	scrn;


void	Spark(){
  int	x, y;
  Window tmpw;
  int   tmpi;
  unsigned int tmpm;

  XQueryPointer( disp, window,
		&tmpw, &tmpw,
		&tmpi, &tmpi,
		&x, &y,
		&tmpm );
  XDrawLine( disp, window, wingc,
	x - 20, y, x - 10, y );
  XDrawLine( disp, window, wingc,
	x + 10, y, x + 20, y );
  XDrawLine( disp, window, wingc,
	x, y - 20 , x, y - 10 );
  XDrawLine( disp, window, wingc,
	x, y + 10 , x, y + 20 );
  XDrawLine( disp, window, wingc,
	x - 20, y - 20, x - 10, y - 10 );
  XDrawLine( disp, window, wingc,
	x + 10, y + 10, x + 20, y + 20 );
  XDrawLine( disp, window, wingc,
	x - 20, y + 20 , x - 10, y + 10 );
  XDrawLine( disp, window, wingc,
	x + 10, y - 10 , x + 20, y - 20 );
}


void	DrawInt( number, x, y )
     int	number;
{
  int	width, height;
  char	letter[ 15 ];

  if( DontCrip( x, y ) ){
    sprintf( letter, "%d", number );
    width = XTextWidth( fontinfo, letter, strlen( letter ) );
    height = fontinfo->ascent + fontinfo->descent;
    XDrawImageString( disp, window, wingc,
		     x - width / 2, y - height / 2 + fontinfo->ascent,
		     letter, strlen( letter ) );
  }
}


void	DrawBoldInt( number, x, y )
     int	number;
{
  int	width, height;
  char	letter[ 15 ];

  if( DontCrip( x, y ) ){
    sprintf( letter, "%d", number );
    width = XTextWidth( fontinfo, letter, strlen( letter ) );
    height = fontinfo->ascent + fontinfo->descent;
    XDrawImageString( disp, window, wingc,
		     x - width / 2, y - height / 2 + fontinfo->ascent,
		     letter, strlen( letter ) );
    XDrawString( disp, window, wingc,
		x - width / 2 + 1, y - height / 2 + fontinfo->ascent,
		letter, strlen( letter ) );
  }
}


void	Circle( x, y, r, pixel, blush )
     int	x, y;
     unsigned int r;
     int	pixel;
     int	blush;
{
  int		i;

  if( DontCrip( x, y ) ){
    XDrawArc( disp, window, wingc, x - (int)r, y - (int)r, 2 * r, 2 * r,
	     0, 23040 );
  }
}


void	Square( x, y, r, pixel, blush )
     int	x, y;
     unsigned int r;
     int	pixel;
     int	blush;
{
  if( DontCrip( x, y ) ){
    XDrawRectangle( disp, window, wingc,
		   x - (int)r, y - (int)r,
		   2 * r, 2 * r );
  }
}


Curve( x0, y0, xf, yf, xp, yp, xt, yt, splineflag )
     int	x0, y0, xf, yf, xp, yp, xt, yt;
     int	splineflag;
{
  XPoint	list[ 4 ];
  
  if( DontCrip( x0, y0 ) && DontCrip( xt, yt ) ){
#ifdef SPLINE
    if( splineflag ){
      list[ 0 ].x = xf;
      list[ 0 ].y = yf;
      list[ 0 ].flags = VertexCurved | VertexStartClosed;
      list[ 1 ].x = xp;
      list[ 1 ].y = yp;
      list[ 1 ].flags = VertexCurved;
      list[ 2 ].x = xt;
      list[ 2 ].y = yt;
      list[ 2 ].flags = VertexCurved | VertexEndClosed;
      XDraw( window, list, 3, EDGEBLUSH, EDGEBLUSH,
	    BlackPixel, GXcopy, AllPlanes );
    }
    else{
      XLine( window, xf, yf, xt, yt,
	    EDGEBLUSH, EDGEBLUSH, BlackPixel, GXcopy, AllPlanes );
    }
#else
#ifdef LINE
    if( splineflag ){
      list[ 0 ].x = xf;
      list[ 0 ].y = yf;
      list[ 1 ].x = xp;
      list[ 1 ].y = yp;
      list[ 2 ].x = xt;
      list[ 2 ].y = yt;
      XDrawLines( disp, window, wingc, list, 3, CoordModeOrigin );
    }
    else{
      XDrawLine( disp, window, wingc, xf, yf, xt, yt );
    }
#else
    XLine( window, xf, yf, xt, yt,
	  EDGEBLUSH, EDGEBLUSH, BlackPixel, GXcopy, AllPlanes );
#endif
#endif
  }
}


void	Cross( x, y, s, brush )
     int	x, y, s;
     int	brush;
{
  if( DontCrip( x, y ) ){
    XDrawLine( disp, window, wingc, x - s, y - s, x + s, y + s );
    XDrawLine( disp, window, wingc, x - s, y + s, x + s, y - s );
  }
}


void	Line( x1, y1, x2, y2 )
     int	x1, y1, x2, y2;
{
  if( DontCrip( x1, y1 ) && DontCrip( x2, y2 ) ){
    XDrawLine( disp, window, wingc, x1, y1, x2, y2 );
  }
}


  
