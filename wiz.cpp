#include "drawinterface.hpp"
#include "flyerz.hpp"
#include "wiz.hpp"

#include <algorithm>

//
// author: Kovacs Marton
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


//
// (ship : ammo) color theme for teams, 0 for default
//
Color teamColors[3][2] = {{Colors::red, Colors::green}, {Colors::red, Colors::green}, {Colors::blue, Colors::white}};

#include <iostream>

Wiz::Wiz()
{
  ships.push_back(new DiskShip(Coordinate(330, 300), Colors::red, *this, 1));
  ships.push_back(new DiskShip(Coordinate(70, 70), Colors::blue, *this, 2));
  ships.push_back(new DiskShip(Coordinate(500, 500), Colors::yellow, *this, 3));
}

Wiz::~Wiz()
{
  delete ships[0];
}

void Wiz::DrawFrame()
{
  static int i=0;

  DrawCircle(Coordinate(200, 200), 100, Color(255, 125, 0), true);
  DrawCircle(Coordinate(210, 210), 100, Color(0, 255, 0), false);

  DrawLine(Coordinate(300 + i % 100, 300), Coordinate(400, 500), Color(0, 255, 255));

  Coordinate points[] = {Coordinate(100, 100), Coordinate(300, 100), Coordinate(300, 300), Coordinate(100, 300)};
  DrawShape(&points[0], &points[4], Color(0, 255, 0), false);
  ++i;

  MoveAll();
  Clean();
}

bool Wiz::CheckCollision(const Coordinate& begin, const Coordinate& end, int team) const
{
  Coordinate vektor = end - begin;
  double len = Length(vektor);
  int steps = len / 2;
  for (int i = 0; i < steps; ++i)
  {
    Coordinate point = begin + vektor * 2 * steps / len;
    for(ShipList::const_iterator it = ships.begin(); ships.end() != it; ++it)
    {
      if ((*it)->Alive() && ((0 == team || team != (*it)->GetTeam()) && Distance((*it)->GetCenter(), point) <= (*it)->GetSize()))
      {
        std::cout << "ouch!\n";
        (*it)->Hit();
        return true;
      }
    }
  }
  return false;
}

void Wiz::AddProjectile(Flyer* projectile)
{
  projectiles.push_back(projectile);
}

void Wiz::RemoveProjectile(Flyer* projectile)
{
  ProjectileList::iterator iter = std::find(projectiles.begin(), projectiles.end(), projectile);
  if (projectiles.end() != iter)
  {
    deads.push_back(projectile);
  }
}

const int Margin = 70;

Coordinate Wiz::PlaceMe(int team) const
{
  Size screenSize = GetSize();
  if (1 == team)
  {
    return Coordinate(Margin + Random(100) - 50, Margin + Random(screenSize.y - 2 * Margin));
  }
  if (2 == team)
  {
    return Coordinate(screenSize.x - 70 + Random(100) - 50, screenSize.y - 70 + Random(screenSize.y - 2 * Margin));
  }
  return Coordinate(Margin + Random(screenSize.x - 2 * Margin), 70 + Random(screenSize.y - 2 * Margin));
}

Wiz::ShipTravel Wiz::GetEnemies(int team) const
{
  ShipTravel res;
  for (ShipList::const_iterator it = ships.begin(); ships.end() != it; ++it)
  {
    const int cteam = (*it)->GetTeam();
    if ((*it)->Alive() && (0 == team || cteam != team))
    {
      res.push_back(*it);
    }
  }
  return res;
}

void Wiz::MoveAll()
{
  //TODO: C++11 feature
  for(ShipList::iterator it = ships.begin(); ships.end() != it; ++it)
  {
    (*it)->Draw();
    (*it)->Move();
  }

  for(ProjectileList::iterator it = projectiles.begin(); projectiles.end() != it; ++it)
  {
    (*it)->Draw();
    (*it)->Move();
  }
  std::cout << '\n';
}

void Wiz::Clean()
{
  for (ProjectileList::iterator it = deads.begin(); deads.end() != it; ++it)
  {
    KillProjectile(*it);
  }
  deads.clear();
}

void Wiz::KillProjectile(Flyer* projectile)
{
  ProjectileList::iterator iter = std::find(projectiles.begin(), projectiles.end(), projectile);
  if (projectiles.end() != iter)
  {
    delete *iter;
    projectiles.erase(iter);
  }
}

