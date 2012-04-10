#include "drawinterface.h"

//
// author: Kovács Márton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// remake of the great blaster screensaver, with more features i missed when watching it
// list of planned features:
//                            teams (so that red and green actually means something)
//                            score
//                            better(?) AI
//                            
//

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

    DiskShip(Coordinate center, Color color, int team = 0): Flyer(team), m_center(center), m_color(color), m_speed(2, 3)
    {}
    virtual void Draw()
    {
      DrawCircle(m_center, shipSize, m_color, true);
    }
    virtual void Move()
    {
      Size size = GetSize();
      m_center.x = (m_center.x + m_speed.x) % size.x;
      m_center.y = (m_center.y + m_speed.y) % size.y;
    }

  private:
    Coordinate  m_center;
    Color       m_color;
    Speed       m_speed;
};

int DiskShip::shipSize = 7;

void DrawFrame()
{
  static Flyer* ship = new DiskShip(Coordinate(500, 500), Color(255, 0, 0), 10);
  static int i=0;

  DrawCircle(Coordinate(200, 200), 100, Color(255, 125, 0), true);
  DrawCircle(Coordinate(210, 210), 100, Color(0, 255, 0), false);

  DrawLine(Coordinate(300 + i%100, 300), Coordinate(400, 500), Color(0, 255, 255));

  Coordinate points[] = {Coordinate(100, 100), Coordinate(300, 100), Coordinate(300, 300), Coordinate(100, 300)};
  DrawShape(&points[0], &points[4], Color(0, 255, 0), false);
  ++i;
  ship->Move();
  ship->Draw();
}

