#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "defs.h"

#include "yubi.ic"
#include "yubi_mask.ic"
#include "wait.ic"
#include "wait_mask.ic"

#define DEFAULTWIDTH	500
#define DEFAULTHEIGHT	600


unsigned int	BDDIOwidth, BDDIOheight;

       Display	*disp;
       Window	window;
       GC	wingc;
       XFontStruct	*fontinfo;
static Cursor	yubicu, waitcu;
       int	scrn;
static int      cripx0, cripx1, cripy0, cripy1;


void	SetCrippingWindow( x0, y0, x1, y1 )
     unsigned int	x0, y0, x1, y1;
{
  cripx0 = x0;
  cripy0 = y0;
  cripx1 = x1;
  cripy1 = y1;
}


static void	ClearWindow(){
  XClearWindow( disp, window );
}


void	ResetWindowSize(){
  XWindowAttributes	info;

  XGetWindowAttributes( disp, window, &info );
  if( info.width != BDDIOwidth || info.height != BDDIOheight ){
    BDDIOwidth = info.width;
    BDDIOheight = info.height;
    SetCrippingWindow( 0, 0, BDDIOwidth, BDDIOheight );
    ClearWindow();
  }
  LocationResetLocation( BDDIOwidth, BDDIOheight );
}


void	QueryColor( foreground, background )
     int	*foreground, *background;
{
  *foreground = BlackPixel( disp, scrn );
  *background = WhitePixel( disp, scrn );
}


static void	DefineCursor(){
  static XColor   frground = {0L, 0, 0, 0};
  static XColor   bground = {0L, 65535, 65535, 65535};
  Pixmap   pix, maskpix;

  pix = XCreateBitmapFromData( disp, window,
			      yubi_bits, yubi_width, yubi_height );
  maskpix = XCreateBitmapFromData( disp, window,
				  yubi_mask_bits,
				  yubi_mask_width, yubi_mask_height );
/*  pix = XCreatePixmapFromBitmapData( disp, window,
				    yubi_bits, yubi_width, yubi_height,
				    BlackPixel( disp, scrn ),
				    WhitePixel( disp, scrn ),
				    1 );
  maskpix = XCreatePixmapFromBitmapData( disp, window,
				    yubi_bits, yubi_width, yubi_height,
				    BlackPixel( disp, scrn ),
				    WhitePixel( disp, scrn ),
				    1 );
*/  
  yubicu = XCreatePixmapCursor( disp, pix, maskpix,
			       &frground, &bground,
			       (unsigned int)yubi_x_hot,
			       (unsigned int)yubi_y_hot );
  XFreePixmap( disp, pix );
  XFreePixmap( disp, maskpix );

  pix = XCreateBitmapFromData( disp, window,
			      wait_bits, wait_width, wait_height );
  maskpix = XCreateBitmapFromData( disp, window,
				  wait_mask_bits,
				  wait_mask_width, wait_mask_height );
  waitcu = XCreatePixmapCursor( disp, pix, maskpix,
			       &frground, &bground,
			       (unsigned int)wait_x_hot,
			       (unsigned int)wait_y_hot );
  XFreePixmap( disp, pix );
  XFreePixmap( disp, maskpix );
}


static void	KillCursor(){
  XFreeCursor( disp, yubicu );
  XFreeCursor( disp, waitcu );
}


int 	WindowOpen( server, fontname )
     char	*server;
     char	*fontname;
{
  char	geo[ 50 ], defgeo[ 50 ];
  char	fname[ 50 ];
  
  disp = XOpenDisplay( server );
  if( disp == NULL ){
    fprintf( stderr, "Can't open display '%s.'\n", server );
    return( FALSE );
  }
  scrn = XDefaultScreen( disp );
  if( fontname == NULL || *fontname == '\0' ){
    strcpy( fname, "fixed" );
  }
  else{
    strcpy( fname, fontname );
  }
  fontinfo = XLoadQueryFont( disp, fname );
  if( fontinfo == NULL ){
    fprintf( stderr, "Can't open font '%s.'\n", fname );
    return( FALSE );
  }
  if( BDDIOwidth == 0 || BDDIOheight == 0 ){
    BDDIOwidth = DEFAULTWIDTH;
    BDDIOheight = DEFAULTHEIGHT;
    SetCrippingWindow( 0, 0, BDDIOwidth, BDDIOheight );
    LocationResetLocation( BDDIOwidth, BDDIOheight );
  }
  window = XCreateSimpleWindow( disp, XRootWindow( disp, scrn ),
		      0, 0, BDDIOwidth, BDDIOheight, 3,
		      BlackPixel( disp, scrn ), WhitePixel( disp, scrn ) );
  XSelectInput( disp, window, ButtonPressMask | ExposureMask | KeyPressMask );

  XStoreName( disp, window, "BDD viewer" );
  wingc = XCreateGC( disp, window, 0, NULL );
  XSetState( disp, wingc,
	    BlackPixel( disp, scrn ), WhitePixel( disp, scrn ),
	    GXcopy, AllPlanes );
  XSetFont( disp, wingc, fontinfo->fid );
  XSetArcMode( disp, wingc, ArcChord );
  DefineCursor();
  XDefineCursor( disp, window, waitcu );
  XMapWindow( disp, window );
  return( TRUE );
}


void	WindowClose(){
  Spark();
  XSync( disp, 0 );
  KillCursor();
  XDestroyWindow( disp, window );
  XFreeFont( disp, fontinfo );
  XCloseDisplay( disp );
}
     

void	Wait(){
  XEvent	event;
  int		exitf;
  static char	message[] = " Beware of X's bug!";

  exitf = 0;
  do{
    XNextEvent( disp, &event );
    switch( event.type ){
    case Expose:
      XDefineCursor( disp, window, waitcu );
      Show();
#ifdef VERBOSE
      XDrawImageString( disp, window, wingc,
		       0, 0 + fontinfo->ascent, message, strlen( message ) );
#endif
      XDefineCursor( disp, window, yubicu );
      break;
    case ButtonPress:
      exitf = 1;
      break;
    }
  }while( exitf == 0 );
}


int	MeanFontWidth(){
  return( fontinfo->min_bounds.width );
}


int	DontCrip( x, y )
     int	x, y;
{
  return( ( x >= cripx0 ) & ( x <= cripx1 )
	 & ( y >= cripy0 ) & ( y <= cripy1 ) );
}


Interrupt(){
  int  b;
  XEvent  event;

  XSync( disp, 0 );
/*  b = XCheckWindowEvent( window, ExposeWindow, &event );
  if( b == 1 ){
    XPutBackEvent( &event );
  }
  return( b );*/
  return( 0 );
}


  
