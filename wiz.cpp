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

    DiskShip(Coordinate center, int team = 0): Flyer(team), m_center(center)
    {}
    virtual void Draw()
    {
    }

  private:
    Coordinate m_center;
};

int DiskShip::shipSize = 15;

void DrawFrame()
{
  static int i=0;

  DrawCircle(Coordinate(200, 200), 100, Color(255, 125, 0));

  DrawLine(Coordinate(300, 300), Coordinate(400, 500), Color(0, 255, 255));

  Coordinate points[] = {Coordinate(100, 100), Coordinate(300, 100), Coordinate(300, 300), Coordinate(100, 300)};
  DrawShape(&points[0], &points[4], Color(0, 255, 0), false);

  if (!(++i % 100))
    ClearScreen();
}

