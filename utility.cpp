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

Coordinate operator*(const Coordinate::CoordType& lhs, const Coordinate& rhs)
{
  return rhs * lhs;
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

Coordinate::CoordType Length(const Coordinate& vektor)
{
  return std::sqrt(LengthSqr(vektor));
}

Coordinate::CoordType LengthSqr(const Coordinate& vektor)
{
  return Sqr(vektor.x) + Sqr(vektor.y);
}

Coordinate::CoordType Distance(const Coordinate& lhs, const Coordinate& rhs)
{
  return Length(lhs - rhs);
}

Coordinate::CoordType DistanceSqr(const Coordinate& lhs, const Coordinate& rhs)
{
  return LengthSqr(lhs - rhs);
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

Coordinate::CoordType Dot(const Coordinate& lhs, const Coordinate& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

const std::string usage = " --teamnum/-n \"N n1 n2 n3 ... nK\" [--size/-s WIDTHxHEIGHT] [--log <file>] [--time <time limit in seconds>] [--score <score limit>] [--font <font name>] [remote ai names, - for builtin]*";

#define RETURN_WITH_USAGE std::cerr << "usage: " << argv[0] << usage << '\n'; return false;

bool ParseCommandline(int argc, char* argv[], Options& options)
{
  opterr = 0;

  int playerCount = 0;

  bool teamflag = false;

  option longOptions[] = {
    {"size", required_argument, 0, 's'},
    {"teamnum", required_argument, 0, 'n'},
    {"log", required_argument, 0, 'l'},
    {"time", required_argument, 0, 't'},
    {"score", required_argument, 0, 'p'},
    {"font", required_argument, 0, 'f'}
  };
  int optionIndex;
  int c;
  while (-1 != (c = getopt_long(argc, argv, "m:s:n:", longOptions, &optionIndex)))
  {
    switch (c)
    {
      case 's':
      {
        int width;
        int height;
        char c;
        std::istringstream str(optarg);
        str >> width >> c >> height;

        if (str.fail())
        {
          std::cerr << "invalid argument for --size: " << optarg << '\n';
          RETURN_WITH_USAGE;
        }

        options.size.x = width;
        options.size.y = height;
        break;
      }
      case 'n':
      {
        int teamnum;
        std::istringstream str(optarg);
        str >> teamnum;
        options.teams.reserve(teamnum);
        for (int i = 0; i < teamnum; ++i)
        {
          int num;
          str >> num;
          options.teams.push_back(num);
          playerCount += num;
        }

        if (!str)
        {
          std::cerr << "invalid argument for --teamnum: " << optarg << '\n';
          RETURN_WITH_USAGE;
        }

        teamflag = true;

        break;
      }
      case 'l':
      {
        options.logFile = optarg;

        break;
      }
      case 't':
      {
        options.time = std::atoi(optarg);

        break;
      }
      case 'p':
      {
        options.score = std::atoi(optarg);

        break;
      }
      case 'f':
      {
        options.fontName = optarg;

        break;
      }
      case '?':
      {
        RETURN_WITH_USAGE;
      }
    }
  }

  options.names.insert(options.names.end(), &argv[optind], &argv[argc]);

  if (!teamflag)
  {
    RETURN_WITH_USAGE;
  }

  for (int i = options.names.size(); i < playerCount; ++i)
  {
    options.names.push_back("-");
  }

  return true;
}

