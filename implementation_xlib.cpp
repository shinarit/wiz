//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// implementation_xlib.cpp
//
// the drawing implementation using xlib
//

#include <vector>
#include <boost/shared_ptr.hpp>
#include <map>
#include <cstdlib>
#include <ctime>

#include <X11/Xlib.h>       // Every Xlib program must include this
#include <X11/keysym.h>     //keysyms

#include <unistd.h>         //usleep
#include <sys/stat.h>       //mkfifo


std::ofstream wizlog("/home/tetra/wizlog");

const char FontName[] = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";// "-adobe-helvetica-medium-o-normal--14-100-100-100-p-78-iso8859-1";

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

State state;

////////////////////////////////////////////////////////////////////////////////////////////////////

void Init(int argc, char *argv[]);
unsigned long Draw();
void Reshape(unsigned int w, unsigned int h);
void Free();

unsigned long TranslateColor(Color color);

int main(int argc, char* argv[])
{
  std::srand(std::time(0));

  Init(argc, argv);

  bool exit = false;
  bool drawable = false;

  while (!exit)
  {
    //event handling part
    XEvent event;
    while (!exit && XPending(state.dpy))
    {
      XNextEvent(state.dpy, &event);
      switch (event.type)
      {
        case MapNotify:
        {
          drawable = true;
          break;
        }
        case ConfigureNotify:
        {
          Reshape(event.xconfigure.width, event.xconfigure.height);
          break;
        }
        case ClientMessage:
        {
          exit = true;
          break;
        }
        case KeyPress:
        {
          if (XK_Escape == XLookupKeysym(&event.xkey, 0))
          {
            exit = true;
          }
          break;
        }
        default:
        {
          break;
        }
      }
    }

    //drawing part
    if (!exit && drawable)
    {
      unsigned long delay = Draw();
      XFlush(state.dpy);
      usleep(delay);
    }
  }

  Free();

  XCloseDisplay(state.dpy);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct MwmHints
{
  unsigned long flags;
  unsigned long functions;
  unsigned long decorations;
  long input_mode;
  unsigned long status;
};

enum
{
  MWM_HINTS_FUNCTIONS = (1L << 0),
  MWM_HINTS_DECORATIONS =  (1L << 1),

  MWM_FUNC_ALL = (1L << 0),
  MWM_FUNC_RESIZE = (1L << 1),
  MWM_FUNC_MOVE = (1L << 2),
  MWM_FUNC_MINIMIZE = (1L << 3),
  MWM_FUNC_MAXIMIZE = (1L << 4),
  MWM_FUNC_CLOSE = (1L << 5)
};

void Init(int argc, char* argv[])
{
  Options options;

  if (!ParseCommandline(argc, argv, options))
  {
    exit(1);
  }

  // Open the display
  Display *dpy = XOpenDisplay(0);
  state.dpy = dpy;

  // Create the window

  Size resolution;
  if (options.demo)
  {
    if (0 != options.size.x)
    {
      resolution = options.size;
    }
    else
    {
      resolution = Size(800, 600);  //default size
    }
  }
  else
  {
    resolution = Size(XDisplayWidth(state.dpy, DefaultScreen(state.dpy)), XDisplayHeight(state.dpy, DefaultScreen(state.dpy)));
  }

  state.window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, resolution.x, resolution.y, 0, BlackPixel(dpy, DefaultScreen(dpy)), BlackPixel(dpy, DefaultScreen(dpy)));

  Atom mwmHintsProperty = XInternAtom(state.dpy, "_MOTIF_WM_HINTS", 0);
  struct MwmHints hints;
  hints.flags = MWM_HINTS_DECORATIONS;
  hints.decorations = 0;
  XChangeProperty(state.dpy, state.window, mwmHintsProperty, mwmHintsProperty, 32, PropModeReplace, (unsigned char *)&hints, 5);

  XWindowAttributes xgwa;
  XGetWindowAttributes(state.dpy, state.window, &xgwa);
  state.cmap = xgwa.colormap;
  state.depth = xgwa.depth;
  state.width = resolution.x;
  state.height = resolution.y;

  XGCValues gcv;
  gcv.foreground = state.fg = TranslateColor(Colors::white);
  gcv.background = state.bg = TranslateColor(Colors::black);
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

  XSelectInput(state.dpy, state.window, StructureNotifyMask | KeyPressMask);

  Atom wm_delete_window = XInternAtom(state.dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(state.dpy, state.window, &wm_delete_window, 1);

  XMapWindow(state.dpy, state.window);

  state.wiz = new Wiz();
  state.wiz->Init(options);
}

unsigned long Draw()
{
//  XSetForeground(state.dpy, state.gc, BlackPixelOfScreen(DefaultScreenOfDisplay(state.dpy)));
  XSetForeground(state.dpy, state.gc, TranslateColor(Colors::black));
  XFillRectangle(state.dpy, state.double_buffer, state.gc, 0, 0, state.width, state.height);

  state.wiz->DrawFrame();
/*
  XWindowAttributes xgwa;
  XGetWindowAttributes (state.dpy, state.window, &xgwa);
*/
  XCopyArea(state.dpy, state.double_buffer, state.window, state.gc, 0, 0, state.width, state.height, 0, 0);

  return state.delay;
}

void Reshape(unsigned int w, unsigned int h)
{
  XWindowAttributes xgwa;
  XGetWindowAttributes (state.dpy, state.window, &xgwa);

  state.width = xgwa.width;
  state.height = xgwa.height;
  state.depth = xgwa.depth;
  state.cmap = xgwa.colormap;

  XFreePixmap(state.dpy, state.double_buffer);
  state.double_buffer = XCreatePixmap(state.dpy, state.window, state.width, state.height, state.depth);
  state.draw = state.double_buffer;
}

void Free()
{
}

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

void DrawWrapper::DrawCircle(Coordinate center, int size, Color color, bool fill)
{
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

void DrawWrapper::DrawLine(Coordinate begin, Coordinate end, Color color)
{
  SetColor(color);
  XDrawLine(state.dpy, state.draw, state.gc, begin.x, begin.y, end.x, end.y);
}

void DrawWrapper::DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
{
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

int DrawWrapper::Random(int sup)
{
  return std::rand() % sup;
}

Size DrawWrapper::GetSize()
{
  return Size(state.width, state.height);
}

int DrawWrapper::DrawTextCentered(const std::string& text, Coordinate center, Color color, int correction)
{
  int res = correction;
  if (-1 == res)
  {
    res = XTextWidth(state.font, text.c_str(), text.size()) / 2;
  }
  SetColor(color);
  XDrawString(state.dpy, state.draw, state.gc, center.x - res, center.y - 1, text.c_str(), text.size());
  return res;
}

void DrawWrapper::DrawText(const std::string& text, Coordinate botleft, Color color)
{
  SetColor(color);
  XDrawString(state.dpy, state.draw, state.gc, botleft.x, botleft.y, text.c_str(), text.size());
}

struct IpcImplementation
{
  IpcImplementation(const std::string& name): outName(name + "_info"), inName(name + "_command"), logName(name + "_log")
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

//    std::cerr << "running " << name + ' ' + inName + ' ' + outName + " &\n";
    std::system((name + ' ' + inName + ' ' + outName + " >" + logName + " 2>/dev/null &").c_str());

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
  const std::string logName;

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

