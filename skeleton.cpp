//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// skeleton.cpp
//
// implementation file of the skeleton class
//

#include "skeleton.hpp"

#include <cmath>
#include <string>
#include <sstream>

#include <iostream>

Skeleton::Skeleton(std::istream& in, std::ostream& out): m_in(in), m_out(out), first(true)
{
  m_out << std::unitbuf;
}

void Skeleton::Begin()
{
  if (!GetAck("begin"))
  {
    ;
  }

  if (first)
  {
    m_out << "get context\n";

    std::string str = GetLine();
    std::istringstream istr(str);

    istr >> str;
    if ("context" != str)
    {
      ;
    }

    istr >> sizeX >> sizeY >> shipSize >> shipSpeed >> bulletLimit >> cooldown >> laserLength >> laserSpeed >> deadTimer;

    first = false;
  }
}

int Skeleton::GetTeam()
{
  m_out << "get team\n";

  std::string str = GetLine();
  std::istringstream istr(str);

  istr >> str;
  if ("team" != str)
  {
    ;
  }

  int team;
  istr >> team;

  return team;
}

Coordinate Skeleton::GetSpeed()
{
  m_out << "get speed\n";

  Coordinate res;
  GetCoordinate(res, "speed");

  return res;
}

bool Skeleton::SetSpeed(const Coordinate& speed)
{
  m_out << "speed " << speed.x << ' ' << speed.y << '\n';

  return GetAck();
}

Coordinate Skeleton::GetCenter()
{
  m_out << "get coords\n";

  Coordinate res;
  if (!GetCoordinate(res, "coords"))
  {
    ;
  }

  return res;
}

bool Skeleton::Shoot(const Coordinate& target)
{
  m_out << "shoot " << target.x << ' ' << target.y << '\n';
  return GetAck();
}

Skeleton::ShipList Skeleton::GetEnemies()
{
  ShipList res;

  m_out << "get enemies\n";
  std::string str = GetLine();
  std::istringstream istr(str);

  istr >> str;
  if ("enemies" != str)
  {
    ;
  }

  int team;
  Coordinate pos;
  while (istr)
  {
    istr >> team >> pos.x >> pos.y;
    if (istr)
    {
      res.push_back(std::make_pair(team, pos));
    }
  }

  return res;
}

Skeleton::ShipList Skeleton::GetTeammates()
{
  ShipList res;

  m_out << "get friends\n";
  std::string str = GetLine();
  std::istringstream istr(str);

  istr >> str;
  if ("friends" != str)
  {
    ;
  }

  Coordinate pos;
  while (istr)
  {
    istr >> pos.x >> pos.y;
    if (istr)
    {
      res.push_back(std::make_pair(-1, pos));
    }
  }

  return res;
}

Skeleton::LaserList Skeleton::GetBullets()
{
  LaserList res;

  m_out << "get bullets\n";
  std::string str = GetLine();
  std::istringstream istr(str);

  istr >> str;
  if ("bullets" != str)
  {
    ;
  }

  int team;
  Coordinate front;
  Coordinate back;
  while (istr)
  {
    istr >> team >> front.x >> front.y >> back.x >> back.y;
    if (istr)
    {
      res.push_back(std::make_pair(team, std::make_pair(front, back)));
    }
  }

  return res;
}

void Skeleton::End()
{
  m_out << "end\n";
}

std::string Skeleton::GetLine()
{
  std::string str;
  std::getline(m_in, str);
  return str;
}

bool Skeleton::GetAck(const std::string& txt)
{
  return txt == GetLine();
}

bool Skeleton::GetCoordinate(Coordinate& coord, const std::string& txt)
{
  std::string str = GetLine();
  std::istringstream istr(str);
  istr >> str >> coord.x >> coord.y;

  return str == txt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////                           ////////////////////////////////////////////////////////////
/////////////  Coordinate helper stuff  ////////////////////////////////////////////////////////////
/////////////                           ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


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
