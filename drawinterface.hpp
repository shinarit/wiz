//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// drawinterface.hpp
//
// common stuff, and the function declarations needed by the drawing logic
//

#ifndef WIZ_DRAW_H_
#define WIZ_DRAW_H_

const int FramePerSecond = 25;
const int FontHeight = 12;

#include <vector>
#include <string>

//
// interface file for drawing layer functions
//

typedef unsigned char uchar;

//simple RGB structure, [0 - 255] range
struct Color
{
  explicit Color(uchar red, uchar green, uchar blue): red(red), green(green), blue(blue)
  {}
  uchar red;
  uchar green;
  uchar blue;
};

namespace Colors
{
  const Color black(0, 0, 0);
  const Color white(255, 255, 255);
  const Color red(255, 0, 0);
  const Color green(0, 255, 0);
  const Color blue(0, 0, 255);
  const Color teal(0, 255, 255);
  const Color yellow(255, 255, 0);
  const Color pink(255, 0, 255);
  const Color purple(0x80, 0, 0x80);
  const Color orange(255, 0xA5, 0);
  const Color gold(0xFD, 0xD0, 0x17);
  const Color lime(0x41, 0xA3, 0x17);
  const Color gray(0xC0, 0xC0, 0xC0);
  const Color grey(gray);
}

//simple class containing the x, y coordinates
struct Coordinate
{
  typedef double CoordType;
  explicit Coordinate(CoordType x = CoordType(), CoordType y = CoordType()): x(x), y(y)
  {}
  CoordType x;
  CoordType y;

  Coordinate& operator+=(const Coordinate& rhs);
  Coordinate& operator*=(const Coordinate::CoordType& rhs);
  Coordinate& operator/=(const Coordinate::CoordType& rhs);
};

Coordinate operator-(const Coordinate& op);
Coordinate operator+(const Coordinate& lhs, const Coordinate& rhs);
Coordinate operator-(const Coordinate& lhs, const Coordinate& rhs);
Coordinate operator*(const Coordinate& lhs, const Coordinate::CoordType& rhs);
Coordinate operator/(const Coordinate& lhs, const Coordinate::CoordType& rhs);

Coordinate::CoordType Length(const Coordinate& vektor);
Coordinate::CoordType Distance(const Coordinate& lhs, const Coordinate& rhs);
Coordinate Normalize(const Coordinate& vektor, Coordinate::CoordType length);
Coordinate Rotate90Cw(const Coordinate& vektor);
Coordinate Rotate90Ccw(const Coordinate& vektor);

typedef Coordinate Size;

struct Options
{
  bool demo;
  Size size;
  std::vector<int> teams;
  std::vector<std::string> names;
};

bool ParseCommandline(int argc, char* argv[], Options& options);

//
// implementation.cpp's interface
//


namespace DrawWrapper
{
  void DrawCircle(Coordinate center, int size, Color col, bool fill = false);
  void DrawLine(Coordinate begin, Coordinate end, Color col);
  void DrawShape(Coordinate* begin, Coordinate* end, Color color, bool fill = false);
  int Random(int sup);
  Size GetSize();
  int DrawTextCentered(const std::string& text, Coordinate center, Color color, int correction = -1);
  void DrawText(const std::string& text, Coordinate botleft, Color color);
}

namespace RemoteProtocol
{
  const std::string BEGIN               = "begin";
  const std::string COMMAND_SPEED       = "speed";
  const std::string COMMAND_SHOOT       = "shoot";

  const std::string QUERY               = "get";
  const std::string QUERY_SPEED         = "speed";
  const std::string QUERY_POSITION      = "coords";
  const std::string QUERY_TEAM          = "team";
  const std::string QUERY_ENEMIES       = "enemies";
  const std::string QUERY_FRIENDS       = "friends";
  const std::string QUERY_BULLETS       = "bullets";
  const std::string QUERY_CONTEXT       = "context";

  const std::string RESPONSE_SPEED      = QUERY_SPEED;
  const std::string RESPONSE_POSITION   = QUERY_POSITION;
  const std::string RESPONSE_TEAM       = QUERY_TEAM;
  const std::string RESPONSE_ENEMIES    = QUERY_ENEMIES;
  const std::string RESPONSE_FRIENDS    = QUERY_FRIENDS;
  const std::string RESPONSE_BULLETS    = QUERY_BULLETS;
  const std::string RESPONSE_CONTEXT    = QUERY_CONTEXT;

  const std::string ACK                 = "ack";
  const std::string DEAD                = "dead";
  const std::string END                 = "end";
}

class IpcImplementation;

class Ipc
{
  public:
    Ipc(const std::string& name);
    ~Ipc();

    //
    // msg should NOT include \n
    //
    void Send(const std::string& msg);
    std::string Receive();

  private:
    IpcImplementation* m_impl;
};

#endif
