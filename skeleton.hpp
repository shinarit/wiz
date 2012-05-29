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

/*
    int GetTeam() const
    {
      return m_ship->GetTeam();
    }

    Wiz::ShipTravel GetEnemies() const
    {
      Wiz::ShipTravel enemies = m_ship->m_frame.GetEnemies(m_ship->GetTeam());
      RemoveMe(enemies, m_ship);
      return enemies;
    }
    Wiz::ShipTravel GetTeammates() const
    {
      Wiz::ShipTravel mates = m_ship->m_frame.GetTeam(m_ship->GetTeam());
      RemoveMe(mates, m_ship);
      return mates;
    }
*/

class Skeleton
{
  public:
    typedef std::vector<std::pair<int, Coordinate> > ShipList;

    Skeleton(std::istream &in, std::ostream &out);

    void Begin();

    Coordinate GetSpeed();
    void SetSpeed(const Coordinate& speed);
    Coordinate GetCenter();
    void Shoot(const Coordinate& target);
    ShipList GetEnemies();
    ShipList GetTeammates();

    void End();


  private:
    bool GetAck(const std::string& txt = "ack");
    bool GetCoordinate(Coordinate& coord, const std::string& txt);

    std::istream& m_in;
    std::ostream& m_out;
};

#endif // SKELETON_H
