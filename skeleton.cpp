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

Skeleton::Skeleton(std::istream& in, std::ostream& out): m_in(in), m_out(out)
{
  m_out << std::unitbuf;
}

void Skeleton::Begin()
{
  if (!GetAck("begin"))
  {
    ;
  }
}

Coordinate Skeleton::GetSpeed()
{
  //m_out << "get speed\n";
  m_out.flush();

  Coordinate res;
  GetCoordinate(res, "speed");

  return res;
}

void Skeleton::SetSpeed(const Coordinate& speed)
{
  m_out << "speed " << speed.x << ' ' << speed.y << '\n';
  m_out.flush();

  if (!GetAck())
  {
    ;
  }
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

void Skeleton::Shoot(const Coordinate& target)
{
  m_out << target.x << ' ' << target.y << '\n';
  if (!GetAck())
  {
    ;
  }
}

Skeleton::ShipList Skeleton::GetEnemies()
{
  ShipList res;
  m_out << "get enemies\n";
  std::string str;
  std::getline(m_in, str);
  std::istringstream istr(str);

  istr >> str;
  if ("enemies" != str)
  {
    ;
  }
  return res;
}

Skeleton::ShipList Skeleton::GetTeammates()
{
  ShipList res;
  return res;
}

void Skeleton::End()
{
  m_out << "end\n";
}


bool Skeleton::GetAck(const std::string& txt)
{
  std::string str;
  std::getline(m_in, str);
  return txt == str;
}

bool Skeleton::GetCoordinate(Coordinate& coord, const std::string& txt)
{
  std::string str;
  std::getline(m_in, str);
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
