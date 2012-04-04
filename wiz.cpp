#include "drawinterface.h"

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

