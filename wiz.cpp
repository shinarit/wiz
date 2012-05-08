#include "drawinterface.hpp"
#include "flyerz.hpp"
#include "wiz.hpp"

#include <algorithm>

#include <iostream>

//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// wiz.cpp
//
// the main class that represents and moves the draw logic
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
Color teamColors[3][2] = {{Colors::red, Colors::green}, {Colors::red, Colors::green}, {Colors::blue, Colors::red}};

#include <iostream>

Wiz::Wiz()
{
}

Wiz::~Wiz()
{
  delete ships[0];
}

void Wiz::Init()
{
  Ipc ipc("test");
  DiskShip* shipptr = new DiskShip(PlaceMe(1), teamColors[1][0], teamColors[1][1], *this, 1);
  DiskShipAi* aiptr = new DiskShipAiRemote(shipptr, ipc);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);
/*
  shipptr = new DiskShip(PlaceMe(1), teamColors[1][0], teamColors[1][1], *this, 1);
  aiptr = new DiskShipAiRanger(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(1), teamColors[1][0], teamColors[1][1], *this, 1);
  aiptr = new DiskShipAiRandom(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(2), teamColors[2][0], teamColors[2][1], *this, 2);
  aiptr = new DiskShipAiRanger(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(2), teamColors[2][0], teamColors[2][1], *this, 2);
  aiptr = new DiskShipAiRandom(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(2), teamColors[2][0], teamColors[2][1], *this, 2);
  aiptr = new DiskShipAiRanger(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);*/
}


void Wiz::DrawFrame()
{
  static int i=0;
/*
  DrawCircle(Coordinate(200, 200), 100, Color(255, 125, 0), true);
  DrawCircle(Coordinate(210, 210), 100, Color(0, 255, 0), false);

  DrawLine(Coordinate(300 + i % 100, 300), Coordinate(400, 500), Color(0, 255, 255));

  Coordinate points[] = {Coordinate(100, 100), Coordinate(300, 100), Coordinate(300, 300), Coordinate(100, 300)};
  DrawShape(&points[0], &points[4], Color(0, 255, 0), true);
*/  ++i;

  MoveAll();
  Clean();
}

bool Wiz::CheckCollision(const Coordinate& begin, const Coordinate& end, int team) const
{
  Coordinate vektor = end - begin;
  Coordinate::CoordType len = Length(vektor);
  int steps = len;
  Coordinate step = vektor / len;
  for (int i = 0; i < steps; ++i)
  {
    Coordinate point = begin + step * i;
    for(ShipList::const_iterator it = ships.begin(); ships.end() != it; ++it)
    {
      if ((*it)->Alive() && ((0 == team || team != (*it)->GetTeam()) && Distance((*it)->GetCenter(), point) <= (*it)->GetSize()))
      {
        //std::cout << "ouch!\n";
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

Coordinate Wiz::PlaceMe(int team) const
{
  Size screenSize = GetSize();
  if (1 == team)
  {
    return Coordinate(Margin + Random(100) - 50, Margin + Random(screenSize.y - 2 * Margin));
  }
  if (2 == team)
  {
    return Coordinate(screenSize.x - 70 + Random(100) - 50, 70 + Random(screenSize.y - 2 * Margin));
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

