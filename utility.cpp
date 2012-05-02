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
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <getopt.h>

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

//
// usage: argv[0] --mode/-m {fullscreen, demo} [--size/-s WIDTHxHEIGHT] [--teamnum/-n N n1 n2 n3...]
//

const std::string usage = "usage: argv[0] --mode/-m {fullscreen, demo} [--size/-s WIDTHxHEIGHT] [--teamnum/-n N n1 n2 n3...]";

#define RETURN_WITH_USAGE std::cerr << usage << '\n'; return false;

bool ParseCommandline(int argc, char* argv[], Options& options)
{
  const std::string fullscreen = "fullscreen";
  const std::string demo = "demo";

  option longOptions[] = {
    {"mode", required_argument, 0, 'm'},
    {"size", required_argument, 0, 's'},
    {"teamnum", required_argument, 0, 'n'}
  };
  int optionIndex;
  int c;
  while (-1 != (c = getopt_long(argc, argv, "m:s:n:", longOptions, &optionIndex)))
  {
    std::string arg(optarg);
    switch (c)
    {
      case 'm':
      {
        if (optarg == fullscreen)
        {
          options.demo = false;
        }
        else if (optarg == demo)
        {
          options.demo = true;
        }
        else
        {
          std::cerr << "invalid argument for --fullscreen: " << arg << '\n';
          RETURN_WITH_USAGE;
        }
        break;
      }
      case 's':
      {
        std::cerr << "size argument: " << arg << '\n';

        int width;
        int height;
        char c;
        std::istringstream str(arg);
        str >> width >> c >> height;

        if (str.bad())
        {
          std::cerr << "invalid argument for --size: " << arg << '\n';
          RETURN_WITH_USAGE;
        }

        options.size.x = width;
        options.size.y = height;
        break;
      }
      case 'n':
      {

        int teamnum;
        std::istringstream str(arg);
        str >> teamnum;
        std::cerr << "teamnum: " << teamnum << '\n';
        options.teams.reserve(teamnum);
        for (int i = 0; i < teamnum; ++i)
        {
          int num;
          str >> num;
          options.teams.push_back(num);
        }

        if (str.bad())
        {
          std::cerr << "invalid argument for --teamnum: " << arg << '\n';
          RETURN_WITH_USAGE;
        }

        break;
      }
    }
  }

  return true;
}

