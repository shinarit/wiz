//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// skeleton.hpp
//
// skeleton for communicating with the game server
// cobra style
//

#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <utility>
#include <istream>
#include <ostream>

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

class Skeleton
{
  public:
    typedef std::vector<std::pair<int, Coordinate> > ShipList;
    typedef std::vector<std::pair<int, std::pair<Coordinate, Coordinate> > > LaserList;

    Skeleton(std::istream &in, std::ostream &out);

    void Begin();

    int GetTeam();
    Coordinate GetSpeed();
    bool SetSpeed(const Coordinate& speed);
    Coordinate GetCenter();
    bool Shoot(const Coordinate& target);
    ShipList GetEnemies();
    ShipList GetTeammates();
    LaserList GetBullets();

    void End();

    int sizeX;
    int sizeY;
    int shipSize;
    int shipSpeed;
    int bulletLimit;
    int cooldown;
    int laserLength;
    int laserSpeed;
    int deadTimer;

  private:
    std::string GetLine();
    bool GetAck(const std::string& txt = "ack");
    bool GetCoordinate(Coordinate& coord, const std::string& txt);

    std::istream& m_in;
    std::ostream& m_out;

    bool first;
};

#endif // SKELETON_H
