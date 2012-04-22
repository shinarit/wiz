//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// flyerz.hpp
//
// anything that can be animated or collided with
//

#ifndef FLYERZ_H
#define FLYERZ_H

#include "drawinterface.hpp"

class Wiz;


class Flyer
{
  public:
    Flyer(int team, Wiz& frame): m_frame(frame), m_ticker(0), m_team(team)
    {}
    virtual ~Flyer()
    {}
    virtual void Draw() = 0;
    virtual void Move() = 0;

    int GetTeam() const
    {
      return m_team;
    }

  protected:
    Wiz&        m_frame;
    int         m_ticker;

  private:
    const int   m_team;
};

class Hitable: public Flyer
{
  public:
    Hitable(int team, Wiz& frame): Flyer(team, frame)
    {}
    virtual Coordinate GetCenter() const = 0;
    virtual Coordinate::CoordType GetSize() const = 0;
    virtual void Hit() = 0;
    virtual bool Alive() = 0;
};

class DiskShipAi;

class DiskShip: public Hitable
{
  public:
    static int shipSize;
    static int maxSpeed;
    static int bulletLimit;
    static int cooldown;
    static int laserLength;
    static int deadInterval;
    static int explosionInterval;
    static int explosionSize;

    DiskShip(Coordinate center, Color shipColor, Color laserColor, Wiz& frame, int team = 0);
    //from Flyer
    virtual void Draw();
    virtual void Move();
    //from Hitable
    virtual Coordinate GetCenter() const;
    virtual Coordinate::CoordType GetSize() const;
    virtual void Hit();
    virtual bool Alive();

  private:
    friend class DiskShipAi;
    void Shoot(const Coordinate& target);

    Coordinate  m_center;
    Color       m_shipColor;
    Color       m_laserColor;
    Coordinate  m_speed;

    int         m_bulletNum;
    int         m_cooldown;
    int         m_dead;
    int         m_explode;

    DiskShipAi* m_ai;
};

class DiskShipAi
{
  public:
    static int minDistance;
    static int maxDistance;

    DiskShipAi(DiskShip* toLead): m_ship(toLead), m_randum(0, 0)
    {}
    void Do();

  private:
    DiskShip* m_ship;

    Coordinate m_randum;
};

class PulseLaser: public Flyer
{
  public:
    static int speed;

    PulseLaser(Coordinate begin, Coordinate end, Color color, Wiz& frame, int team = 0): Flyer(team, frame), m_front(begin), m_back(end), m_speed((m_front - m_back)), m_color(color)
    {
      m_speed = (m_speed * speed) / Length(m_speed);
    }
    //from Flyer
    virtual void Draw();
    virtual void Move();

  private:
    Coordinate  m_front;
    Coordinate  m_back;
    Coordinate  m_speed;
    Color       m_color;
};


#endif // FLYERZ_H
