#ifndef FLYERZ_H
#define FLYERZ_H

#include "drawinterface.hpp"

class Flyer
{
  public:
    Flyer(int team): m_team(team)
    {}
    virtual void Draw() = 0;
    virtual void Move() = 0;
    virtual ~Flyer()
    {}

    int GetTeam() const
    {
      return m_team;
    }

  private:
    const int m_team;
};

class DiskShip: public Flyer
{
  public:
    static int shipSize;
    static int bulletLimit;

    DiskShip(Coordinate center, Color color, int team = 0): Flyer(team), m_center(center), m_color(color), m_speed(2, 3), m_bulletNum(0)
    {}
    virtual void Draw()
    {
      DrawCircle(m_center, shipSize, m_color, true);
    }
    virtual void Move()
    {
      Size screenSize = GetSize();

      m_center.x = (m_center.x + m_speed.x) % screenSize.x;
      m_center.y = (m_center.y + m_speed.y) % screenSize.y;

      if(m_center.x + shipSize > screenSize.x)
        m_center.x = screenSize.x - shipSize;
      if(m_center.y + shipSize > screenSize.y)
        m_center.y = screenSize.y - shipSize;
      if(m_center.x - shipSize < 0)
        m_center.x = 0 + shipSize;
      if(m_center.y - shipSize < 0)
        m_center.y = 0 + shipSize;
    }

  private:
    Coordinate  m_center;
    Color       m_color;
    Coordinate  m_speed;

    int         m_bulletNum;
};

class PulseLaser: public Flyer
{
  public:
    static int pulseLaserSpeed;
    PulseLaser(Coordinate begin, Coordinate end, Color color, int team = 0): Flyer(team), m_front(begin), m_back(end), m_speed((m_front - m_back)*pulseLaserSpeed), m_color(color)
    {}
    virtual void Draw()
    {
      DrawLine(m_front, m_back, m_color);
    }
    virtual void Move()
    {
      m_front += m_speed;
      //collision check
      m_back += m_speed;
    }

  private:
    Coordinate  m_front;
    Coordinate  m_back;
    Coordinate  m_speed;
    Color       m_color;
};


#endif // FLYERZ_H
