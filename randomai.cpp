//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// randomai.cpp
//
// sample ai which copies the builtin random ai
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

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

inline Coordinate::CoordType Sqr(Coordinate::CoordType x)
{
  return x*x;
}

Coordinate::CoordType Length(const Coordinate& vektor)
{
  return std::sqrt(Sqr(vektor.x) + Sqr(vektor.y));
}

Coordinate operator*(const Coordinate& lhs, const Coordinate::CoordType& rhs)
{
  return Coordinate(lhs.x * rhs, lhs.y * rhs);
}

Coordinate operator/(const Coordinate& lhs, const Coordinate::CoordType& rhs)
{
  return Coordinate(lhs.x / rhs, lhs.y / rhs);
}

Coordinate operator+(const Coordinate& lhs, const Coordinate& rhs)
{
  return Coordinate(lhs.x + rhs.x, lhs.y + rhs.y);
}

Coordinate& Coordinate::operator+=(const Coordinate& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Coordinate Normalize(const Coordinate& vektor, Coordinate::CoordType length)
{
  return vektor * length / Length(vektor);
}


int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <commands file> <data file>\n";
    return 1;
  }

  std::ifstream in(argv[2]);
  std::ofstream out(argv[1]);

  if (!out || !in)
  {
    std::cerr << "Error with opening file\n";
    return 2;
  }
/*
  if (!(GetTicker() % changeDirectionInterval))
  {
    m_randum = Normalize(Coordinate(DrawWrapper::Random(1000) - 500, DrawWrapper::Random(1000) - 500), DiskShip::maxSpeed);
  }
  GetSpeed() += m_randum;

  //shooting if feasible
  Wiz::ShipTravel enemies = GetEnemies();
  if (!enemies.empty())
  {
    const Hitable* enemy = 0;
    if (!m_target || (!(GetTicker() % changeTargetInterval)))
    {
      enemy = enemies[DrawWrapper::Random(enemies.size())];
    }
    //found enemy. so shoot
    if (enemy)
    {
      Shoot(enemy->GetCenter());
    }
  }
*/
  std::srand(std::time(0) + getpid());

  const int changeDirectionInterval = 10;
  const int changeTargetInterval = 3;
  const int maxSpeed = 5;
  int ticker = 0;

  Coordinate randum;

  std::string str;
  while (true)
  {
    std::getline(in, str);  //begin
    //////////////////
    if (!(ticker++ % changeDirectionInterval))
    {
      randum = Normalize(Coordinate(std::rand()%1000 - 500, std::rand()%1000 - 500), maxSpeed);
    }
    Coordinate speed;
    out << "get speed\n";
    out.flush();
    std::getline(in, str);  //speed x y
    std::istringstream istr(str);
    istr >> str >> speed.x >> speed.y;
    speed += randum;
    out << "speed " << speed.x << ' ' << speed.y << '\n';
    out.flush();
    //////////////////

    std::getline(in, str);  //ack
    out << "end\n";
    out.flush();
  }

}
