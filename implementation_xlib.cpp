#include <vector>
#include <boost/shared_ptr.hpp>
#include <map>

std::ofstream wizlog("/home/tetra/wizlog");

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

const char FontName[] = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";// "-adobe-helvetica-medium-o-normal--14-100-100-100-p-78-iso8859-1";
extern int globalArgc;
extern char** globalArgv;

struct State
{
  Display* dpy;
  Window window;

  GC gc;
  int delay;
  unsigned long fg, bg;
  int width, height;
  int depth;
  Colormap cmap;
  XFontStruct* font;

  Drawable draw;

  Wiz* wiz;

  //double buffering support
  Pixmap double_buffer;
};

State* stateptr = 0;

void* wiz_init(Display* dpy, Window window)
{
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

  XGCValues gcv;
  gcv.foreground = state.fg = get_pixel_resource(state.dpy, state.cmap, "foreground", "Foreground");
  gcv.background = state.bg = get_pixel_resource(state.dpy, state.cmap, "background", "Background");
  gcv.fill_style = FillSolid;

  Font fid = XLoadFont(state.dpy, FontName);
  state.font = XQueryFont(state.dpy, fid);
  if (!state.font)
  {
    std::cerr << "Could not load font: " << FontName << '\n';
    std::exit(1);
  }
  gcv.font = fid;

  state.gc = XCreateGC (state.dpy, state.window, GCForeground | GCBackground | GCFillStyle | GCFont, &gcv);

  state.delay = 1000000 / FramePerSecond; //1000000 == 1 second

  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, state.depth);
  state.draw = state.double_buffer;
  //state.draw = state.window;

  Options options;

  if (!ParseCommandline(globalArgc, globalArgv, options))
  {
    exit(1);
  }

  state.wiz->Init(options);
  return tostore;
}

unsigned long wiz_draw(Display* dpy, Window window, void* closure)
{
  State& state = *static_cast<State*>(closure);
  stateptr = &state;

  XSetForeground(state.dpy, state.gc, BlackPixelOfScreen(DefaultScreenOfDisplay(state.dpy)));
  XFillRectangle(state.dpy, state.double_buffer, state.gc, 0, 0, state.width, state.height);

  state.wiz->DrawFrame();
  XWindowAttributes xgwa;
  XGetWindowAttributes (dpy, window, &xgwa);

  XCopyArea(state.dpy, state.double_buffer, state.window, state.gc, 0, 0, state.width, state.height, 0, 0);

  return state.delay;
}

const char* wiz_defaults[] =
{
  ".background:	black",
  ".foreground:	white",
  "*fpsSolid:	true",
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
  State& state = *static_cast<State*>(closure);

  XWindowAttributes xgwa;
  XGetWindowAttributes (dpy, window, &xgwa);

  state.width = xgwa.width;
  state.height = xgwa.height;
  state.depth = xgwa.depth;
  state.cmap = xgwa.colormap;
  //state.dpy = dpy;
  //state.window = window;

  XFreePixmap(dpy, state.double_buffer);
  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, state.depth);
  state.draw = state.double_buffer;
  //state.draw = state.window;
}

int wiz_event(Display* dpy, Window window, void* closure, XEvent *event)
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

struct CompareColors
{
  bool operator()(const XColor& lhs, const XColor& rhs)
  {
    return  lhs.red < rhs.red ||
            (lhs.red == rhs.red && lhs.green < rhs.green) ||
            (lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue < rhs.blue);
  }
};

unsigned long TranslateColor(Color color)
{
  State& state = *stateptr;

  CompareColors comp;
  typedef std::map<XColor, unsigned long, CompareColors> ColorMap;
  static ColorMap ColorCache(comp);

  XColor xcol = {0, 257*color.red, 257*color.green, 257*color.blue, DoRed | DoGreen | DoBlue, 0};
  ColorMap::iterator colorIt = ColorCache.find(xcol);

  if (ColorCache.end() == colorIt)
  {
    XAllocColor(state.dpy, state.cmap, &xcol);
    ColorCache[xcol] = xcol.pixel;
  }
  else
  {
    xcol.pixel = colorIt->second;
  }
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

void DrawText(const std::string& text, Coordinate center, Color color)
{
  State& state = *stateptr;

  SetColor(color);
  XDrawString(state.dpy, state.draw, state.gc, center.x - XTextWidth(state.font, text.c_str(), text.size()) / 2, center.y - 1, text.c_str(), text.size());
}


struct IpcImplementation
{
  IpcImplementation(const std::string& name): outName(name + "_info"), inName(name + "_command")
  {
    std::remove(outName.c_str());
    std::remove(inName.c_str());
    if (-1 == mkfifo(outName.c_str(), 0666))
    {
      throw "Could not open " + outName;
    }
    if (-1 == mkfifo(inName.c_str(), 0666))
    {
      throw "Could not open " + inName;
    }

    m_out.reset(new std::ofstream(outName.c_str()));
    m_in.reset(new std::ifstream(inName.c_str()));
  }

  ~IpcImplementation()
  {
    if (m_out.unique())
    {
      std::remove(outName.c_str());
      std::remove(inName.c_str());
    }
  }

  const std::string outName;
  const std::string inName;

  boost::shared_ptr<std::ostream> m_out;
  boost::shared_ptr<std::istream> m_in;
};

Ipc::Ipc(const std::string& name): m_impl(new IpcImplementation(name))
{ }

Ipc::~Ipc()
{ }

void Ipc::Send(const std::string& msg)
{
  *m_impl->m_out << msg << '\n';
  (*m_impl->m_out).flush();
}

std::string Ipc::Receive()
{
  std::string res;
  std::getline(*m_impl->m_in, res);
  return res;
}

