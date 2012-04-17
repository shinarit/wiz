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
    virtual Coordinate GetCenter() = 0;
    virtual Coordinate::CoordType GetSize() = 0;
};

class DiskShip: public Hitable
{
  public:
    static int shipSize;
    static int bulletLimit;
    static int cooldownInterval;
    static int laserLength;

    DiskShip(Coordinate center, Color color, Wiz& frame, int team = 0): Hitable(team, frame), m_center(center), m_color(color), m_speed(2, 3), m_bulletNum(0)
    {}
    virtual void Draw();
    virtual void Move();
    virtual Coordinate GetCenter();
    virtual Coordinate::CoordType GetSize();

  private:
    friend class Wiz;
    void Shoot();

    Coordinate  m_center;
    Color       m_color;
    Coordinate  m_speed;

    int         m_bulletNum;
};

class PulseLaser: public Flyer
{
  public:
    static int speed;

    PulseLaser(Coordinate begin, Coordinate end, Color color, Wiz& frame, int team = 0): Flyer(team, frame), m_front(begin), m_back(end), m_speed((m_front - m_back)), m_color(color)
    {
      m_speed = (m_speed * speed) / Length(m_speed);
    }
    virtual void Draw();
    virtual void Move();

  private:
    Coordinate  m_front;
    Coordinate  m_back;
    Coordinate  m_speed;
    Color       m_color;
};


#endif // FLYERZ_H
