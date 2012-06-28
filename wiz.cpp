//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// wiz.cpp
//
// the main class that represents and moves the draw logic
//

#include "drawinterface.hpp"
#include "flyerz.hpp"
#include "wiz.hpp"
#include "ais.hpp"

#include <climits>
#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>

#include <iostream>

const std::string randomNames[] =
{
  "robot0x00",        "Stanley",  "GAURRR",   "muszmusz",   "d(O_O)b",          "Bob",          "Greyson",    "Robert Paulson",   "Jeronimo",             "Suzuki",
  "robot0xbfffffff",  "Utvefuro", "|o|",      "|oo|",       "<o>",              "Roy",          "Jaime",      "Diablo",           "RAMPAGE",              "PENTAKILL",
  "robot0xcfcfcfcf",  "vidya",    ">oo>",     "<oo<",       "isten, csak igy",  "n00b",         "l33+h4xxoR", "Faust",            "gut, besser, gosser",  "Hovertank",
  "CrayII",           "AVENGER",  "blaster",  "SKYRANGER",  "LIGHTNING",        "INTERCEPTOR",  "FIRESTORM",  "Sleeper Service",  "Zero Gravitas",        "Very Little Gravitas Indeed"
};

Wiz::Wiz()
{
}

Wiz::~Wiz()
{
  //TODO
}

void Wiz::Init(const Options& options)
{
  logFile = options.logFile;
  timeLimit = ((-1 != options.time) ? (options.time) : (INT_MAX));
  scoreLimit = ((-1 != options.score) ? (options.score) : (INT_MAX));
  startTime = std::time(0);

  Margin = std::min(DrawWrapper::GetSize().x, DrawWrapper::GetSize().y) / 10;

  std::vector<std::string>::const_iterator nit = options.names.begin();

  int teamCounter = 1;
  if (1 == options.teams.size())
  {
    teamCounter = 0;
  }

  int id = 0;

  std::set<std::string> nameBin(&randomNames[0], &randomNames[sizeof(randomNames)/sizeof(randomNames[0])]);

  for (std::vector<int>::const_iterator tit = options.teams.begin(); options.teams.end() != tit; ++tit)
  {
    teamScores.push_back(ScoreSorter());
    for (int i = 0; i < *tit; ++i)
    {
      std::string name = *nit++;
      bool randomAi = false;
      if (name == "-")
      {
        if (nameBin.empty())
        {
          name = "---- not enough names ----";
        }
        else
        {
          std::set<std::string>::iterator rname = nameBin.begin();
          std::advance(rname, DrawWrapper::Random(nameBin.size()));
          name = *rname;
          nameBin.erase(rname);
        }
        randomAi = true;
      }

      DiskShip* shipPtr = new DiskShip(PlaceMe(teamCounter), GetShipColor(teamCounter), GetLaserColor(teamCounter), name, id++, *this, teamCounter);
      DiskShipAi* aiPtr;
      if (randomAi)
      {
        aiPtr = new DiskShipAiRandom(shipPtr);
      }
      else
      {
        Ipc ipc(name);
        //TODO create ipc
        aiPtr = new DiskShipAiRemote(shipPtr, ipc);
      }
      shipPtr->SetAi(aiPtr);
      ships.push_back(shipPtr);
      scores.push_back(0);
      teamScores.back().insert(ScoreType(&scores[i+1], ships.back()));
    }
    ++teamCounter;
  }

//  scores = ScoreList(ships.size(), 0);
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
  DrawScore();
  if (timeLimit < std::time(0) - startTime)
  {
    std::cout << "Time is up\n";
    ShutDown();
  }
}

bool Wiz::CheckCollision(const Coordinate& begin, const Coordinate& end, int team, int owner) const
{
  Coordinate vektor = begin - end;
  Coordinate::CoordType len = Length(vektor);

  //get the potential guys
  ShipList potentials = GetPotentials(team, end + vektor / 2, DiskShip::shipSize + (DiskShip::laserLength + 1) / 2);

  //calculating the step
  Coordinate step = vektor * CheckDistance / len;

  bool hit = false;

  for (ShipList::const_iterator it = potentials.begin(); potentials.end() != it; ++it)
  {
    Coordinate point = end;
    for(int i(0); i < len / CheckDistance; ++i)
    {
      if (Distance((*it)->GetCenter(), point) <= (*it)->GetSize())
      {
        (*it)->Hit();
        if (scoreLimit <= ++scores[owner])
        {
          std::cout << "Score limit hit\n";
          ShutDown();
        }
        hit = true;
        break;
      }
      point += step;
    }
  }
  return hit;
}

void Wiz::AddProjectile(Owned* projectile)
{
  projectiles.push_back(projectile);
  projectileCache.push_back(std::make_pair(projectile->GetTeam(), projectile->GetCollision()));
}

void Wiz::RemoveProjectile(Owned *projectile)
{
  ProjectileList::iterator iter = std::find(projectiles.begin(), projectiles.end(), projectile);
  if (projectiles.end() != iter)
  {
    deads.push_back(projectile);
  }
}

Coordinate Wiz::PlaceMe(int team) const
{
  Size screenSize = DrawWrapper::GetSize();
  if (1 == team)
  {
    return Coordinate(Margin + DrawWrapper::Random(100) - 50, Margin + DrawWrapper::Random(screenSize.y - 2 * Margin));
  }
  if (2 == team)
  {
    return Coordinate(screenSize.x - Margin + DrawWrapper::Random(100) - 50, Margin + DrawWrapper::Random(screenSize.y - 2 * Margin));
  }
  return Coordinate(Margin + DrawWrapper::Random(screenSize.x - 2 * Margin), Margin + DrawWrapper::Random(screenSize.y - 2 * Margin));
}

//
// (ship : ammo) color theme for teams, 0 for default
//  0 gets random, also teams above the given limit gets random too
//
const Color teamColors[][2] = {{Colors::red, Colors::green}, {Colors::blue, Colors::red}, {Colors::white, Colors::white}, {Colors::purple, Colors::pink}};
const int PredefNum = sizeof(teamColors)/sizeof(teamColors[0]);

Color GetRandomColor()
{
  return Color(DrawWrapper::Random(255), DrawWrapper::Random(255), DrawWrapper::Random(255));
}

Color Wiz::GetShipColor(int team) const
{
  if (0 == team || PredefNum < team)
  {
    return teamColors[0][0];
    //return GetRandomColor();
  }
  else
  {
    return teamColors[team - 1][0];
  }
}

Color Wiz::GetLaserColor(int team) const
{
  if (0 == team || PredefNum < team)
  {
    return teamColors[0][1];
    //return GetRandomColor();
  }
  else
  {
    return teamColors[team - 1][1];
  }
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

Wiz::LaserList Wiz::GetBullets() const
{
  return projectileCache;
}


void Wiz::MoveAll()
{
  //TODO: C++11 feature
  LaserList::iterator lit = projectileCache.begin();
  for(ProjectileList::iterator it = projectiles.begin(); projectiles.end() != it; ++it)
  {
    (*it)->Draw();
    (*it)->PrepareMove();
    (*it)->ExecuteMove();
    lit->second = (*it)->GetCollision();
    ++lit;
  }

  for(ShipList::iterator it = ships.begin(); ships.end() != it; ++it)
  {
    (*it)->Draw();
    (*it)->PrepareMove();
    (*it)->ExecuteMove();
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

void DrawRectangle(Coordinate topleft, int width, int height, Color color)
{
  Coordinate rect[] = {topleft, topleft + Coordinate(width, 0), topleft + Coordinate(width, height), topleft + Coordinate(0, height)};
  DrawWrapper::DrawShape(&rect[0], &rect[4], color);
}

void Wiz::DrawScore()
{
  /*
  Coordinate startPoint(DrawWrapper::GetSize().x - 100, DrawWrapper::GetSize().y - ships.size() * (FontHeight + 1) + FontHeight / 2);
  if (100 < startPoint.x)
  {
    if (0 > startPoint.y)
    {
      startPoint.y = FontHeight;
    }

    int team = 0;
    if (1 < teamScores.size())
    {
      team = 1;
    }

    std::ostringstream ostr;
    Coordinate topleft = startPoint - Coordinate(1, FontHeight);

    for (TeamScoreList::iterator sit = teamScores.begin(); teamScores.end() != sit; ++sit)
    {
      for (ScoreSorter::iterator pit = sit->begin(); sit->end() != pit; ++pit)
      {
        ostr << pit->GetScore() << ": " << pit->GetName();
        DrawWrapper::DrawText(ostr.str(), startPoint, Colors::pink);
        startPoint += Coordinate(0, FontHeight + 1);
        ostr.str("");
        ostr.clear();
      }
      DrawRectangle(topleft, 100, startPoint.y - topleft.y - FontHeight * 2, teamColors[team][0]);
      ++team;
      topleft = startPoint - Coordinate(1, FontHeight * 2);
    }
    DrawRectangle(topleft, 100, startPoint.y - topleft.y - FontHeight, teamColors[team][0]);
  }
  */

  Coordinate startPoint(DrawWrapper::GetSize().x - 100, DrawWrapper::GetSize().y - ships.size() * (FontHeight + 1) + FontHeight / 2);
  if (100 < startPoint.x)
  {
    if (0 > startPoint.y)
    {
      startPoint.y = FontHeight;
    }
    std::ostringstream ostr;

    int team = ships.front()->GetTeam();
    Coordinate topleft = startPoint - Coordinate(1, FontHeight);
    for (unsigned i = 0; i < ships.size(); ++i)
    {
      ostr << scores[i] << ": " << ships[i]->GetName();
      DrawWrapper::DrawText(ostr.str(), startPoint, Colors::pink);
      startPoint += Coordinate(0, FontHeight + 1);
      ostr.str("");
      ostr.clear();

      if (ships[i]->GetTeam() != team)
      {
        DrawRectangle(topleft, 100, startPoint.y - topleft.y - FontHeight * 2, teamColors[team][0]);

        team = ships[i]->GetTeam();
        topleft = startPoint - Coordinate(1, FontHeight * 2);
      }
    }
    DrawRectangle(topleft, 100, startPoint.y - topleft.y - FontHeight, teamColors[team][0]);
  }
}


void Wiz::KillProjectile(Owned* projectile)
{
  ProjectileList::iterator iter = std::find(projectiles.begin(), projectiles.end(), projectile);
  if (projectiles.end() != iter)
  {
    int pos = iter - projectiles.begin();
    delete *iter;
    projectiles.erase(iter);
    projectileCache.erase(projectileCache.begin() + pos);
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

void Wiz::ShutDown() const
{
  std::cout << "Final score:\n";
  for (int i(0); i < scores.size(); ++i)
  {
    std::cout << ships[i]->GetName() << ": " << scores[i] << '\n';
  }
  std::exit(0);
}

int Wiz::Margin;

int ScoreType::GetScore() const
{
  return *m_score;
}

std::string ScoreType::GetName() const
{
  return m_ship->GetName();
}

bool operator<(const ScoreType& lhs, const ScoreType& rhs)
{
  return lhs.GetScore() > rhs.GetScore();
}
