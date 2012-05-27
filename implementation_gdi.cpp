std::ofstream wizlog("wizlog");

#define _WIN32_WINNT 0x0500

#include <windows.h>

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
  }
  argv[argc] = 0;
  LocalFree(argvw);

  Options options;
  if (!ParseCommandline(argc, argv, options))
  {
    return 0;
  }

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
  wiz.Init(options);
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

void DrawWrapper::DrawCircle(Coordinate center, int size, Color color, bool fill)
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

void DrawWrapper::DrawLine(Coordinate begin, Coordinate end, Color color)
{
  SelectObject(drawhdc, GetStockObject(DC_PEN));
  SelectObject(drawhdc, GetStockObject(DC_BRUSH));
  SetDCPenColor(drawhdc, TranslateColor(color));
  SetDCBrushColor(drawhdc, TranslateColor(color));

  MoveToEx(drawhdc, begin.x, begin.y, 0);
  LineTo(drawhdc, end.x, end.y);
}

void DrawWrapper::DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill)
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

int DrawWrapper::Random(int sup)
{
  return std::rand() % sup;
}

Size DrawWrapper::GetSize()
{
  return size;
}

#undef DrawText

int DrawWrapper::DrawTextCentered(const std::string& text, Coordinate center, Color color, int correction)
{
  RECT rect = {center.x, center.y - 10, center.x, center.y - 10};
  DrawTextA(drawhdc, text.c_str(), text.size(), &rect, DT_INTERNAL | DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
  return 0;
}

void DrawWrapper::DrawText(const std::string& text, Coordinate botleft, Color color)
{
  RECT rect = {botleft.x, botleft.y, botleft.x, botleft.y};
  DrawTextA(drawhdc, text.c_str(), text.size(), &rect, DT_INTERNAL | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
}

struct IpcImplementation
{ };

Ipc::Ipc(const std::string& name): m_impl(0)
{ }

Ipc::~Ipc()
{ }

void Ipc::Send(const std::string& msg)
{ }

std::string Ipc::Receive()
{
  return "END";
}

