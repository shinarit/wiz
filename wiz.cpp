#include "drawinterface.hpp"
#include "flyerz.hpp"
#include "wiz.hpp"

#include <algorithm>
#include <iterator>

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
Color teamColors[][2] = {{Colors::red, Colors::green}, {Colors::red, Colors::green}, {Colors::blue, Colors::red}, {Colors::white, Colors::white}};

Wiz::Wiz()
{
}

Wiz::~Wiz()
{
  delete ships[0];
}

void Wiz::Init(const Options& options)
{
  std::vector<std::string>::const_iterator nit = options.names.begin();
  int nameCounter;

  int teamCounter = 1;
  if (1 == options.teams.size())
  {
    teamCounter = 0;
  }

  std::cerr << "teamnum: " << options.teams.size() << '\n';
  for (std::vector<int>::const_iterator tit = options.teams.begin(); options.teams.end() != tit; ++tit)
  {
    std::cerr << "players in team: " << *tit << '\n';
    for (int i = 0; i < *tit; ++i)
    {
      std::cerr << "player: " << i << '\n';
      std::string name = *nit;
      bool randomAi = false;
      if (name == "-")
      {
        name = "randum " + char('0' + nameCounter++);
        randomAi = true;
      }

      DiskShip* shipPtr = new DiskShip(PlaceMe(teamCounter), teamColors[teamCounter][0], teamColors[teamCounter][1], *nit, *this, teamCounter);
      DiskShipAi* aiPtr;
      if (randomAi)
      {
        aiPtr = new DiskShipAiRandom(shipPtr);
      }
      else
      {
        Ipc ipc("");
        //TODO create ipc
        aiPtr = new DiskShipAiRemote(shipPtr, ipc);
      }
      shipPtr->SetAi(aiPtr);
      std::cerr << "pushin a ship yo!\n";
      ships.push_back(shipPtr);
    }
    ++teamCounter;
  }
/*
  Ipc ipc("test");
  DiskShip* shipptr = new DiskShip(PlaceMe(3), teamColors[3][0], teamColors[3][1], "REMOTE AI        X  I  X        IA ETOMER", *this, 3);
  DiskShipAi* aiptr = new DiskShipAiRemote(shipptr, ipc);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(1), teamColors[1][0], teamColors[1][1], "RedRandom", *this, 1);
  aiptr = new DiskShipAiRandom(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);

  shipptr = new DiskShip(PlaceMe(2), teamColors[2][0], teamColors[2][1], "BlueRandom", *this, 2);
  aiptr = new DiskShipAiRandom(shipptr);
  shipptr->SetAi(aiptr);
  ships.push_back(shipptr);
*/
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
  DrawText("asd", Coordinate(100, 12), Colors::red);

  MoveAll();
  Clean();
}

bool Wiz::CheckCollision(const Coordinate& begin, const Coordinate& end, int team) const
{
  Coordinate vektor = begin - end;
  Coordinate::CoordType len = Length(vektor);

  //get the potential guys
  ShipList potentials = GetPotentials(team, end + vektor / 2, DiskShip::shipSize + (DiskShip::laserLength + 1) / 2);

  //calculating the step
  Coordinate step = vektor * CheckDistance / len;
  for (ShipList::const_iterator it = potentials.begin(); potentials.end() != it; ++it)
  {
    Coordinate point = end;
    for(int i(0); i < len / CheckDistance; ++i)
    {
      if (Distance((*it)->GetCenter(), point) <= (*it)->GetSize())
      {
        (*it)->Hit();
        return true;
      }
      point += step;
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

struct EnemyPredicate
{
  EnemyPredicate(int team, bool negate = false): m_team(team), m_negate(negate)
  { }
  bool operator()(const Hitable* ship)
  {
    return  (m_negate ^ (0 != ship->GetTeam() && ship->GetTeam() == m_team)) || !ship->Alive();
  }

  int   m_team;
  bool  m_negate;
};

Wiz::ShipTravel Wiz::GetEnemies(int team) const
{
  ShipTravel res;
  std::remove_copy_if(ships.begin(), ships.end(), std::back_inserter(res), EnemyPredicate(team));
  return res;
}

Wiz::ShipTravel Wiz::GetTeam(int team) const
{
  ShipTravel res;
  std::remove_copy_if(ships.begin(), ships.end(), std::back_inserter(res), EnemyPredicate(team, true));
  return res;
}


void Wiz::MoveAll()
{
  //TODO: C++11 feature
  std::cerr << "move mah ship yo!\n";
  for(ShipList::iterator it = ships.begin(); ships.end() != it; ++it)
  {
    std::cerr << "an goyo!\n";
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

struct PotentialityChecker
{
  PotentialityChecker(int team, Coordinate center, int dist): m_subpred(team), m_center(center), m_dist(dist)
  { }
  bool operator()(const Hitable* ship)
  {
    return m_subpred(ship) || Distance(ship->GetCenter(), m_center) > m_dist;
  }

  EnemyPredicate m_subpred;
  Coordinate m_center;
  int m_dist;
};


Wiz::ShipList Wiz::GetPotentials(int team, Coordinate center, int dist) const
{
  ShipList res;
  std::remove_copy_if(ships.begin(), ships.end(), std::back_inserter(res), PotentialityChecker(team, center, dist));
  return res;
}

