#include "drawinterface.hpp"

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

Coordinate operator*(const Coordinate& lhs, const int rhs)
{
  return Coordinate(lhs.x * rhs, lhs.y * rhs);
}

Coordinate operator/(const Coordinate& lhs, const int rhs)
{
  return Coordinate(lhs.x / rhs, lhs.y / rhs);
}

Coordinate& Coordinate::operator+=(const Coordinate& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

inline int Sqr(int x)
{
  return x*x;
}

int Length(const Coordinate& vektor)
{
  return std::sqrt(Sqr(vektor.x) + Sqr(vektor.y));
}

