#include "drawinterface.h"

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
// implementation.cpp
// modified from greynetic.c, credit see above
// author: Marton Kovacs, 2012, tetra666@gmail.com
// license: same as above
//

extern "C"
{
#include "screenhack.h"
#include "xdbe.h"
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
  
  //double buffer support
  GC erase_gc;
  Pixmap b, ba, bb;
  Bool dbeclear_p;
  XdbeBackBuffer backb;
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

  //double buffering support
  //have to understand yet
  state.dbeclear_p = get_boolean_resource(state.dpy, "useDBEClear", "Boolean");
  if (state.dbeclear_p)
    state.b = xdbe_get_backbuffer (state.dpy, state.window, XdbeBackground);
  else
    state.b = xdbe_get_backbuffer (state.dpy, state.window, XdbeUndefined);
  state.backb = state.b;

  if (!state.b)
  {
    state.ba = XCreatePixmap (state.dpy, state.window, state.xlim, state.ylim, xgwa.depth);
    state.bb = XCreatePixmap (state.dpy, state.window, state.xlim, state.ylim, xgwa.depth);
    state.b = state.ba;
  }

  state.erase_gc = XCreateGC (state.dpy, state.b, GCForeground, &gcv);

  if (state.ba) XFillRectangle (state.dpy, state.ba, state.erase_gc, 0, 0, state.xlim, state.ylim);
  if (state.bb) XFillRectangle (state.dpy, state.bb, state.erase_gc, 0, 0, state.xlim, state.ylim);

//end of support

  Init();

  return tostore;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
  if (!state.dbeclear_p || !state.backb)
  {
    XFillRectangle(state.dpy, state.b, state.erase_gc, 0, 0, state.xlim, state.ylim);
  }

  XClearWindow(state.dpy, state.window);
  DrawFrame();

  if (state.backb)
  {
    XdbeSwapInfo info[1];
    info[0].swap_window = state.window;
    info[0].swap_action = (state.dbeclear_p ? XdbeBackground : XdbeUndefined);
    XdbeSwapBuffers (state.dpy, info, 1);
  }
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
  "*useDBE:		True",
  "*useDBEClear:	True",
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
  Exit();
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


#elif defined _WIN32

#include <iostream>

#define _WIN32_WINNT 0x0500
#include <windows.h>

const int DrawInterval = 1000/FramePerSecond;

Size size;
HDC hdc;
PAINTSTRUCT paint;

HWND hWnd;
LPCTSTR ClsName = "Wiz";
LPCTSTR WindowCaption = "Wiz";
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void SetHdc();
void Draw();

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
  MSG         Msg;
  WNDCLASSEX  WndClsEx;

  WndClsEx.cbSize        = sizeof(WNDCLASSEX);
  WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
  WndClsEx.lpfnWndProc   = WndProc;
  WndClsEx.cbClsExtra    = 0;
  WndClsEx.cbWndExtra    = 0;
  WndClsEx.hInstance     = hInstance;
  WndClsEx.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
  WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
  WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  WndClsEx.lpszMenuName  = NULL;
  WndClsEx.lpszClassName = ClsName;
  WndClsEx.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);

  RegisterClassEx(&WndClsEx);

  size.x = GetSystemMetrics(SM_CXFULLSCREEN);
  size.y = GetSystemMetrics(SM_CYFULLSCREEN);

  hWnd = CreateWindowEx(WS_EX_STATICEDGE,
                        ClsName,
                        WindowCaption,
                        WS_POPUP,
                        0,
                        0,
                        size.x,
                        size.y,
                        0,
                        0,
                        hInstance,
                        0);

  Init();

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  
  SetTimer(hWnd, 0, DrawInterval, 0);

  while( GetMessage(&Msg, 0, 0, 0) )
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch(Msg)
  {
    case WM_DESTROY:
    {
      PostQuitMessage(WM_QUIT);
      break;
    }
    case WM_KEYDOWN:
    {
      switch(wParam)
      {
        default:
        {
          Exit();
          exit(0);
        }
      }
      break;
    }
    case WM_TIMER:
    {
      //fallthrough
    }
    case WM_PAINT:
    {
      Draw();
      break;
    }
    default:
    {
      return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
  }
  return 0;
}

//
// windows implementation part
//

void SetHdc()
{
  hdc = BeginPaint(hWnd, &paint);
}

void Draw()
{
  InvalidateRect(hWnd, 0, true);
  SetHdc();
  DrawFrame();
}

#define TranslateColor(color) RGB(color.red, color.green, color.blue)

//
// no, really now...
//

void DrawCircle(Coordinate center, int size, Color color, bool fill)
{
  SelectObject(hdc, GetStockObject(DC_PEN));
  SetDCPenColor(hdc, TranslateColor(color));

  if(fill)
  {
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hdc, TranslateColor(color));
    Ellipse(hdc, center.x - size, center.y - size, center.x + size, center.y + size);
  }
  else
  {
    Arc(hdc, center.x - size, center.y - size, center.x + size, center.y + size, center.x, center.y - size, center.x, center.y - size);
  }
}

void DrawLine(Coordinate begin, Coordinate end, Color color)
{
  SelectObject(hdc, GetStockObject(DC_PEN));
  SelectObject(hdc, GetStockObject(DC_BRUSH));
  SetDCPenColor(hdc, TranslateColor(color));

  MoveToEx(hdc, begin.x, begin.y, 0);
  LineTo(hdc, end.x, end.y);
}

void DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
{
  SelectObject(hdc, GetStockObject(DC_PEN));
  SetDCPenColor(hdc, TranslateColor(color));

  if(fill)
  {
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hdc, TranslateColor(color));
    //Ellipse(hdc, center.x - size, center.y - size, center.x + size, center.y + size);
  }
  else
  {
    //Arc(hdc, center.x - size, center.y - size, center.x + size, center.y + size, center.x, center.y - size, center.x, center.y - size);
  }
}


Size GetSize()
{
  return size;
}

void ClearScreen(Color col)
{
}

#endif

