void DrawFrame();

#ifdef __linux__

// xscreensaver, Copyright (c) 1992-2008 Jamie Zawinski <jwz@jwz.org>
//
// Permission to use, copy, modify, distribute, and sell this software and its
// documentation for any purpose is hereby granted without fee, provided that
// the above copyright notice appear in all copies and that both that
// copyright notice and this permission notice appear in supporting
// documentation.  No representations are made about the suitability of this
// software for any purpose.  It is provided "as is" without express or 
// implied warranty.
//

//
// wiz.cpp
// modified from greynetic.c, credit see above
// author: Marton Kovacs, 2012, tetra666@gmail.com
// license: same as above
//

extern "C"
{
#include "screenhack.h"
}

struct State
{
  Display* dpy;
  Window window;

  GC gc;
  int delay;
  unsigned long fg, bg;
  int xlim, ylim;
  bool grey_p;
  Colormap cmap;
};

void* wiz_init(Display *dpy, Window window)
{
//maybe we will need to re-calloc it, dont know who should free this closure
//  State* st = (struct State *) calloc (1, sizeof(*st));
  State* st = new State();

  st->dpy = dpy;
  st->window = window;

  XWindowAttributes xgwa;
  XGetWindowAttributes(st->dpy, st->window, &xgwa);
  st->xlim = xgwa.width;
  st->ylim = xgwa.height;
  st->cmap = xgwa.colormap;
  st->grey_p = get_boolean_resource(st->dpy, "grey", "Boolean");

  XGCValues gcv;
  gcv.foreground= st->fg = get_pixel_resource(st->dpy, st->cmap, "foreground", "Foreground");
  gcv.background= st->bg = get_pixel_resource(st->dpy, st->cmap, "background", "Background");
  gcv.fill_style = FillSolid;

  st->gc = XCreateGC (st->dpy, st->window, GCForeground | GCBackground | GCFillStyle, &gcv);

  st->delay = get_integer_resource(st->dpy, "delay", "Integer");
  if (st->delay < 0)
    st->delay = 0;

  return st;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
  DrawFrame();

  static int sx = 1;
  static int sy = 2;
  static int cx = 0;
  static int cy = 0;
  State* st = reinterpret_cast<State*>(closure);
  int w=0, h=0, i;

  XGCValues gcv;

  XColor fgc, bgc;

  fgc.flags = bgc.flags = DoRed | DoGreen | DoBlue;
  fgc.red = random();
  fgc.green = random();
  fgc.blue = random();
  bgc.red = random();
  bgc.green = random();
  bgc.blue = random();

  if (st->grey_p)
  {
    fgc.green = fgc.blue = fgc.red;
    bgc.green = bgc.blue = bgc.red;
  }
  
  fgc.green = fgc.blue = fgc.red = 65535;
  
  if ((!XAllocColor (st->dpy, st->cmap, &fgc)) || (!XAllocColor (st->dpy, st->cmap, &bgc)))
  {
    st->grey_p = true;
    return st->delay;
  }

  gcv.foreground = fgc.pixel;
  gcv.background = bgc.pixel;

  XChangeGC (st->dpy, st->gc, GCForeground, &gcv);
//  XFillRectangle (st->dpy, st->window, st->gc, cx, cy, w, h);
  XClearWindow(st->dpy, st->window);

for(int i(0); i<1000; ++i)
{
  XDrawArc (st->dpy, st->window, st->gc, (cx + i) % st->xlim, cy % st->ylim, 10, 10, 0, 360*64);
  XFillArc (st->dpy, st->window, st->gc, st->xlim - cx % st->xlim, st->ylim - (cy + i) % st->ylim, 10, 10, 0, 360*64);
}
//  XDrawArc (st->dpy, st->window, st->gc, 10, 10, 10, 10, 0, 360*64);
  cx += sx;
  cy += sy;
  

  return st->delay;
}

const char* wiz_defaults[] =
{
  ".background:	black",
  ".foreground:	white",
  "*fpsSolid:	true",
  "*delay:	10000",
  "*grey:	false",
  0
};

XrmOptionDescRec wiz_options[] =
{
  { "-delay",		".delay",	XrmoptionSepArg, 0 },
  { "-grey",		".grey",	XrmoptionNoArg, "True" },
  { 0, 0, XrmoptionNoArg, 0 }
};

void wiz_reshape(Display *dpy, Window window, void *closure, 
                 unsigned int w, unsigned int h)
{
  State* st = reinterpret_cast<State*>(closure);
  st->xlim = w;
  st->ylim = h;
}

int wiz_event(Display *dpy, Window window, void *closure, XEvent *event)
{
  return false;
}

void wiz_free(Display *dpy, Window window, void *closure)
{
  delete reinterpret_cast<State*>(closure);
}

XSCREENSAVER_MODULE ("Wiz", wiz)


#elif defined _WIN32


#endif

