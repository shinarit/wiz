//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// utility.cpp
//
// implementation of some operations on the common stuff
//

#include "drawinterface.hpp"
#include "flyerz.hpp"

#include <cmath>

Coordinate operator-(const Coordinate& op)
{
  return Coordinate(-op.x, -op.y);
}

Coordinate operator+(const Coordinate& lhs, const Coordinate& rhs)
{
  return Coordinate(lhs.x + rhs.x, lhs.y + rhs.y);
}

Coordinate operator-(const Coordinate& lhs, const Coordinate& rhs)
{
  return lhs + -rhs;
}

Coordinate operator*(const Coordinate& lhs, const Coordinate::CoordType& rhs)
{
  return Coordinate(lhs.x * rhs, lhs.y * rhs);
}

Coordinate operator/(const Coordinate& lhs, const Coordinate::CoordType& rhs)
{
  return Coordinate(lhs.x / rhs, lhs.y / rhs);
}

Coordinate& Coordinate::operator+=(const Coordinate& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Coordinate& Coordinate::operator*=(const Coordinate::CoordType& rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Coordinate& Coordinate::operator/=(const Coordinate::CoordType& rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

inline Coordinate::CoordType Sqr(Coordinate::CoordType x)
{
  return x*x;
}

Coordinate::CoordType Length(const Coordinate& vektor)
{
  return std::sqrt(Sqr(vektor.x) + Sqr(vektor.y));
}

Coordinate::CoordType Distance(const Coordinate& lhs, const Coordinate& rhs)
{
  return Length(lhs - rhs);
}

Coordinate Normalize(const Coordinate& vektor, Coordinate::CoordType length)
{
  return vektor * length / Length(vektor);
}

Coordinate Rotate90Cw(const Coordinate& vektor)
{
  return Coordinate(vektor.y, -vektor.x);
}

Coordinate Rotate90Ccw(const Coordinate& vektor)
{
  return Coordinate(-vektor.y, vektor.x);
}
//#include <unistd.h>
#include <getopt.h>
//
// usage: argv[0] --mode/-m {fullscreen, demo} [--size/-s WIDTHxHEIGHT] [--teamnum/-n N n1 n2 n3...]
//
bool ParseCommandline(int argc, char* argv[])
{
  option longOptions[] = {
    {"mode", required_argument, 0, 'm'},
    {"size", required_argument, 0, 's'},
    {"teamnum", required_argument, 0, 'n'}
  };
  int optionIndex;
  int c;
  while (-1 != (c = getopt_long(argc, argv, "m:s:n:", longOptions, &optionIndex)))
  {
    switch (c)
    {
      case 'm':
      {

        break;
      }
      case 's':
      {
        break;
      }
      case 'n':
      {
        break;
      }
    }
  }
}

