//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// implementation.cpp
//
// the drawing implementation with conditional compiling for different OSs
//

#include "drawinterface.hpp"
#include "wiz.hpp"
#include <iostream>

#include <fstream>
#include <string>
struct printer
{
  printer(const std::string& print, std::ostream& out): print(print), out(out)
  {
    out << "BEGIN " << print << '\n';
    out.flush();
  }
  ~printer()
  {
    out << "END " << print << '\n';
    out.flush();
  }

  std::ostream& out;
  std::string print;
};

std::ofstream wizlog("/home/tetra/wizlog");

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
  int depth;
  bool grey_p;
  Colormap cmap;

  Drawable draw;

  Wiz* wiz;

  //double buffering support
  Pixmap double_buffer;
};

State* stateptr = 0;

void* wiz_init(Display* dpy, Window window)
{
//  printer a("init", wizlog);

  State* tostore = new State();
  stateptr = tostore;
  State& state = *tostore;

  state.wiz = new Wiz();

  state.dpy = dpy;
  state.window = window;

  XWindowAttributes xgwa;
  XGetWindowAttributes(state.dpy, state.window, &xgwa);
  state.width = xgwa.width;
  state.height = xgwa.height;
  state.depth = xgwa.depth;
  state.cmap = xgwa.colormap;
  state.grey_p = get_boolean_resource(state.dpy, "grey", "Boolean");

  XGCValues gcv;
  gcv.foreground= state.fg = get_pixel_resource(state.dpy, state.cmap, "foreground", "Foreground");
  gcv.background= state.bg = get_pixel_resource(state.dpy, state.cmap, "background", "Background");
  gcv.fill_style = FillSolid;

  state.gc = XCreateGC (state.dpy, state.window, GCForeground | GCBackground | GCFillStyle, &gcv);

  state.delay = 1000000 / FramePerSecond; //1000000 == 1 second

  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, state.depth);
  state.draw = state.double_buffer;
  //state.draw = state.window;

  state.wiz->Init();
  return tostore;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
//  printer a("draw", wizlog);

  State& state = *static_cast<State*>(closure);
  stateptr = &state;
  
  XSetForeground(state.dpy, state.gc, BlackPixelOfScreen(DefaultScreenOfDisplay(state.dpy)));
  XFillRectangle(state.dpy, state.double_buffer, state.gc, 0, 0, state.width, state.height);
  
  state.wiz->DrawFrame();
  
  XCopyArea(state.dpy, state.double_buffer, state.window, state.gc, 0, 0, state.width, state.height, 0, 0);

  return state.delay;
}

const char* wiz_defaults[] =
{
  ".background:	black",
  ".foreground:	white",
  "*fpsSolid:	true",
  "*grey:	false",
  0
};

XrmOptionDescRec wiz_options[] =
{
  { "-delay",		".delay",	XrmoptionSepArg, 0 },
  { "-grey",		".grey",	XrmoptionNoArg, "True" },
  { 0, 0, XrmoptionNoArg, 0 }
};
void wiz_reshape(Display* dpy, Window window, void* closure,
                 unsigned int w, unsigned int h)
{
  printer a("reshape", wizlog);

  State& state = *static_cast<State*>(closure);

  XWindowAttributes xgwa;
  XGetWindowAttributes (dpy, window, &xgwa);

  state.width = xgwa.width;
  state.height = xgwa.height;
  state.depth = xgwa.depth;
  state.cmap = xgwa.colormap;
  //state.dpy = dpy;
  //state.window = window;

  wizlog << state.width << " : " << state.height << '\n';


  XFreePixmap(dpy, state.double_buffer);
  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, state.depth);
  state.draw = state.double_buffer;
  //state.draw = state.window;
}

int wiz_event(Display* dpy, Window window, void* closure, XEvent *event)
{
  //printer a("event", wizlog);
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
    XFillArc(state.dpy, state.draw, state.gc, center.x - size, center.y - size, 2 * size, 2 * size, 0, 360*64);
  }
  else
  {
    XDrawArc(state.dpy, state.draw, state.gc, center.x - size, center.y - size, 2 * size, 2 * size, 0, 360*64);
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

int Random(int sup)
{
  return random() % sup;
}

Size GetSize()
{
  State& state = *stateptr;

  return Size(state.width, state.height);
}


#elif defined _WIN32

#define _WIN32_WINNT 0x0500

#include <windows.h>
//#include <scrnsave.h>

#include <cstdlib>
#include <ctime>

const int DrawInterval = 1000/FramePerSecond;

// nasty global variables
Size size;

Wiz* wizptr;

HDC hdc;
HDC bufferhdc;
HBITMAP bitmap;
PAINTSTRUCT paint;

//the pointer to the hdc to draw to
HDC drawhdc;

HWND hWnd;
LPCTSTR ClsName = "Wiz";
LPCTSTR WindowCaption = "Wiz";
bool quit = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void SetHdc();
void Draw();

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
  int argc;
  wchar_t** argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
  char** argv = new char*[argc + 1];

  for (int i(0); i < argc; ++i)
  {
    int len = WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, 0, 0, 0, 0);
    argv[i] = new char[len];
    WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, argv[i], len, 0, 0);
    std::cout << argv[i] << '\n';
  }
  argv[argc] = 0;
  LocalFree(argvw);



  MSG         Msg;
  WNDCLASSEX  WndClsEx;

  WndClsEx.hCursor       = 0; //LoadCursor(NULL, IDC_ARROW);
  WndClsEx.cbSize        = sizeof(WNDCLASSEX);
  WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
  WndClsEx.lpfnWndProc   = WndProc;
  WndClsEx.cbClsExtra    = 0;
  WndClsEx.cbWndExtra    = 0;
  WndClsEx.hInstance     = hInstance;
  WndClsEx.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
  WndClsEx.hbrBackground = 0;//(HBRUSH)GetStockObject(BLACK_BRUSH);
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

  std::srand(std::time(0));

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  
  SetTimer(hWnd, 0, DrawInterval, 0);

  Wiz wiz;
  wiz.Init();
  wizptr = &wiz;

  while( !quit && GetMessage(&Msg, 0, 0, 0) )
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
          quit = true;
          break;
        }
      }
      break;
    }
    case WM_TIMER:
    {
      InvalidateRect(hWnd, 0, true);
      break;
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
  bufferhdc = CreateCompatibleDC(hdc);
  bitmap = CreateCompatibleBitmap(hdc, size.x, size.y);
  SelectObject(bufferhdc, bitmap);
  drawhdc = bufferhdc;
}

void ReleaseHdc()
{
  BitBlt(hdc, 0, 0, size.x, size.y, bufferhdc, 0, 0, SRCCOPY);
  DeleteObject(bitmap);
  DeleteDC(bufferhdc);
  DeleteDC(hdc);
  EndPaint(hWnd, &paint);
}

void Draw()
{
  SetHdc();
  wizptr->DrawFrame();
  ReleaseHdc();
}

std::vector<POINT> ConvertPoints(const Coordinate* const begin, const Coordinate* const end)
{
  std::vector<POINT> result;

  const Coordinate* runptr = begin;
  while(end != runptr)
  {
    POINT p = {runptr->x, runptr->y};
    result.push_back(p);
    ++runptr;
  }
  return result;
}


#define TranslateColor(color) RGB(color.red, color.green, color.blue)

//
// no, really now...
//

void DrawCircle(Coordinate center, int size, Color color, bool fill)
{
  SelectObject(drawhdc, GetStockObject(DC_PEN));
  SetDCPenColor(drawhdc, TranslateColor(color));

  if(fill)
  {
    SelectObject(drawhdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(drawhdc, TranslateColor(color));
    Ellipse(drawhdc, center.x - size, center.y - size, center.x + size, center.y + size);
  }
  else
  {
    Arc(drawhdc, center.x - size, center.y - size, center.x + size, center.y + size, center.x, center.y - size, center.x, center.y - size);
  }
}

void DrawLine(Coordinate begin, Coordinate end, Color color)
{
  SelectObject(drawhdc, GetStockObject(DC_PEN));
  SelectObject(drawhdc, GetStockObject(DC_BRUSH));
  SetDCPenColor(drawhdc, TranslateColor(color));
  SetDCBrushColor(drawhdc, TranslateColor(color));

  MoveToEx(drawhdc, begin.x, begin.y, 0);
  LineTo(drawhdc, end.x, end.y);
}

void DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
{
  SelectObject(drawhdc, GetStockObject(DC_PEN));
  SetDCPenColor(drawhdc, TranslateColor(color));

  std::vector<POINT> points = ConvertPoints(begin, end);
  points.push_back(points.front());

  if(fill)
  {
    SelectObject(drawhdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(drawhdc, TranslateColor(color));
    Polygon(drawhdc, &points[0], points.size());
  }
  else
  {
    Polyline(drawhdc, &points[0], points.size());
  }
}

int Random(int sup)
{
  return std::rand() % sup;
}

Size GetSize()
{
  return size;
}

void ClearScreen(Color col)
{
}

#endif

