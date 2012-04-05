#include "drawinterface.h"

void DrawFrame();

#ifdef __linux__

#include <vector>

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

State state;

void* wiz_init(Display *dpy, Window window)
{
//maybe we will need to re-calloc it, dont know who should free this closure
  State* tostore = new State();

  state.dpy = dpy;
  state.window = window;

  XWindowAttributes xgwa;
  XGetWindowAttributes(state.dpy, state.window, &xgwa);
  state.xlim = xgwa.width;
  state.ylim = xgwa.height;
  state.cmap = xgwa.colormap;
  state.grey_p = get_boolean_resource(state.dpy, "grey", "Boolean");

  XGCValues gcv;
  gcv.foreground= state.fg = get_pixel_resource(state.dpy, state.cmap, "foreground", "Foreground");
  gcv.background= state.bg = get_pixel_resource(state.dpy, state.cmap, "background", "Background");
  gcv.fill_style = FillSolid;

  state.gc = XCreateGC (state.dpy, state.window, GCForeground | GCBackground | GCFillStyle, &gcv);

  state.delay = get_integer_resource(state.dpy, "delay", "Integer");
  if (state.delay < 0)
    state.delay = 0;

  return tostore;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
  DrawFrame();
/*
  static int sx = 1;
  static int sy = 2;
  static int cx = 0;
  static int cy = 0;
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

  if (state.grey_p)
  {
    fgc.green = fgc.blue = fgc.red;
    bgc.green = bgc.blue = bgc.red;
  }
  
  fgc.green = fgc.blue = fgc.red = 65535;
  
  if ((!XAllocColor (state.dpy, state.cmap, &fgc)) || (!XAllocColor (state.dpy, state.cmap, &bgc)))
  {
    state.grey_p = true;
    return state.delay;
  }

  gcv.foreground = fgc.pixel;
  gcv.background = bgc.pixel;

  XChangeGC (state.dpy, state.gc, GCForeground, &gcv);
//  XFillRectangle (state.dpy, state.window, state.gc, cx, cy, w, h);
//  XClearWindow(state.dpy, state.window);

for(int i(0); i<1000; ++i)
{
  XDrawArc (state.dpy, state.window, state.gc, (cx + i) % state.xlim, cy % state.ylim, 10, 10, 0, 360*64);
  XFillArc (state.dpy, state.window, state.gc, state.xlim - cx % state.xlim, state.ylim - (cy + i) % state.ylim, 10, 10, 0, 360*64);
}
//  XDrawArc (state.dpy, state.window, state.gc, 10, 10, 10, 10, 0, 360*64);
  cx += sx;
  cy += sy;
  
*/
  return state.delay;
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
  state.xlim = w;
  state.ylim = h;
}

int wiz_event(Display *dpy, Window window, void *closure, XEvent *event)
{
  return false;
}

void wiz_free(Display *dpy, Window window, void *closure)
{
//  delete reinterpret_cast<State*>(closure);
}

XSCREENSAVER_MODULE ("Wiz", wiz)


//
// beginning of implementation of common stuff
//

unsigned long TranslateColor(Color color)
{
  XColor xcol = {0, 257*color.red, 257*color.green, 257*color.blue, DoRed | DoGreen | DoBlue, 0};
  XAllocColor(state.dpy, state.cmap, &xcol);
  return xcol.pixel;
}

void SetColor(Color color)
{
  XSetForeground(state.dpy, state.gc, TranslateColor(color));
}

std::vector<XPoint> ConvertPoints(const Coordinate* const begin, const Coordinate* const end)
{
  std::vector<XPoint> result;
  
  const Coordinate* runptr = begin;
  while(end != runptr)
  {
    XPoint xp = {runptr->x, runptr->y};
    result.push_back(xp);
    ++runptr;
  }
  return result;
}

//
// now really...
//

void DrawCircle(Coordinate center, int size, Color color, bool fill)
{
  SetColor(color);
  if(fill)
  {
    XFillArc(state.dpy, state.window, state.gc, center.x - size, center.y - size, size, size, 0, 360*64);
  }
  else
  {
    XDrawArc(state.dpy, state.window, state.gc, center.x - size, center.y - size, size, size, 0, 360*64);
  }
}

void DrawLine(Coordinate begin, Coordinate end, Color color)
{
  SetColor(color);
  XDrawLine(state.dpy, state.window, state.gc, begin.x, begin.y, end.x, end.y);
}

void DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
{
  SetColor(color);
  std::vector<XPoint> points = ConvertPoints(begin, end);
  points.push_back(points.front());
  if(fill)
  {
    XFillPolygon(state.dpy, state.window, state.gc, &(points[0]), points.size(), Convex, CoordModeOrigin);
  }
  else
  {
    XDrawLines(state.dpy, state.window, state.gc, &(points[0]), points.size(), CoordModeOrigin);
  }
}


Size GetSize()
{
  return Size(state.xlim, state.ylim);
}

void ClearScreen(Color col)
{
  XClearWindow(state.dpy, state.window);
}



#elif defined _WIN32


#endif

