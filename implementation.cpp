#include "drawinterface.hpp"
#include "wiz.hpp"

//debug tool
#include <iostream>
//

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
}

struct State
{
  Display* dpy;
  Window window;

  GC gc;
  int delay;
  unsigned long fg, bg;
  int width, height;
  bool grey_p;
  Colormap cmap;

  Drawable draw;

  //double buffering support
  Wiz wiz;
  Pixmap double_buffer;
};

State* stateptr = 0;

void* wiz_init(Display *dpy, Window window)
{
  State* tostore = new State();
  State& state = *tostore;

  state.dpy = dpy;
  state.window = window;

  XWindowAttributes xgwa;
  XGetWindowAttributes(state.dpy, state.window, &xgwa);
  state.width = xgwa.width;
  state.height = xgwa.height;
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

  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, xgwa.depth);
  state.draw = state.double_buffer;

  return tostore;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
  State& state = *static_cast<State*>(closure);
  stateptr = &state;
  
  XSetForeground(state.dpy, state.gc, BlackPixelOfScreen(DefaultScreenOfDisplay(state.dpy)));
  XFillRectangle(state.dpy, state.double_buffer, state.gc, 0, 0, state.width, state.height);
  
  state.wiz.DrawFrame();
  
  XCopyArea(state.dpy, state.double_buffer, state.window, state.gc, 0, 0, state.width, state.height, 0, 0);

  return state.delay;
}

const char* wiz_defaults[] =
{
  ".background:	black",
  ".foreground:	white",
  "*fpsSolid:	true",
  "*delay:	40000", //40.000 msec == 25 fps
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
  State& state = *static_cast<State*>(closure);

  state.width = w;
  state.height = h;
}

int wiz_event(Display *dpy, Window window, void *closure, XEvent *event)
{
//  State& state = *static_cast<State*>(closure);

  return false;
}

void wiz_free(Display *dpy, Window window, void *closure)
{
  State& state = *static_cast<State*>(closure);

  delete &state;
}

XSCREENSAVER_MODULE ("Wiz", wiz)


//
// beginning of implementation of common stuff
//

unsigned long TranslateColor(Color color)
{
  State& state = *stateptr;

  XColor xcol = {0, 257*color.red, 257*color.green, 257*color.blue, DoRed | DoGreen | DoBlue, 0};
  XAllocColor(state.dpy, state.cmap, &xcol);
  return xcol.pixel;
}

void SetColor(Color color)
{
  State& state = *stateptr;

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
  State& state = *stateptr;

  SetColor(color);
  if(fill)
  {
    XFillArc(state.dpy, state.draw, state.gc, center.x - size, center.y - size, size, size, 0, 360*64);
  }
  else
  {
    XDrawArc(state.dpy, state.draw, state.gc, center.x - size, center.y - size, size, size, 0, 360*64);
  }
}

void DrawLine(Coordinate begin, Coordinate end, Color color)
{
  State& state = *stateptr;

  SetColor(color);
  XDrawLine(state.dpy, state.draw, state.gc, begin.x, begin.y, end.x, end.y);
}

void DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
{
  State& state = *stateptr;

  SetColor(color);
  std::vector<XPoint> points = ConvertPoints(begin, end);
  points.push_back(points.front());
  if(fill)
  {
    XFillPolygon(state.dpy, state.draw, state.gc, &(points[0]), points.size(), Convex, CoordModeOrigin);
  }
  else
  {
    XDrawLines(state.dpy, state.draw, state.gc, &(points[0]), points.size(), CoordModeOrigin);
  }
}


Size GetSize()
{
  State& state = *stateptr;

  return Size(state.width, state.height);
}


#elif defined _WIN32

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

