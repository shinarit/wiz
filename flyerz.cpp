//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// flyerz.cpp
//
// anything that can be animated or collided with
//

#include "flyerz.hpp"
#include "wiz.hpp"

#include <algorithm>
#include <cmath>

#include <iostream>

struct DistanceComparer
{
  DistanceComparer(Hitable* toCompare): m_comp(toCompare->GetCenter())
  {}
  DistanceComparer(Coordinate toCompare): m_comp(toCompare)
  {}
  bool operator()(const Hitable* lhs, const Hitable* rhs)
  {
    return Distance(lhs->GetCenter(), m_comp) < Distance(rhs->GetCenter(), m_comp);
  }

  Coordinate m_comp;
};

//
// DiskShip functions
//

DiskShip::DiskShip(Coordinate center, Color color, Color lasercolor, Wiz& frame, int team): Hitable(team, frame), m_center(center), m_shipColor(color), m_laserColor(lasercolor), m_bulletNum(0), m_cooldown(0), m_dead(0), m_ai(0)
{}

const Color ExplosionColors[] = {Colors::yellow, Colors::orange};

void DiskShip::Draw()
{
  if (m_dead)
  {
    if (m_explode)
    {
      if (2 != Random(3))   //67% of showing some explosion
      {
        Color color = ExplosionColors[Random(2)];
        int size = explosionSize / 2 + Random(explosionSize / 2) * (m_explode / 2);

        DrawCircle(m_center, size, color, true);

        --m_explode;
      }
    }
  }
  else
  {
    DrawCircle(m_center, shipSize, m_shipColor, true);
  }
}

void DiskShip::Move()
{
  ++m_ticker;
  if (0 < m_cooldown)
  {
    --m_cooldown;
  }

  if (m_dead)
  {
    if (0 == --m_dead)
    {
      m_center = m_frame.PlaceMe(GetTeam());
    }
  }
  else
  {
    Size screenSize = ::GetSize();

    m_center.x = (m_center.x + m_speed.x);
    m_center.y = (m_center.y + m_speed.y);

    if(m_center.x + shipSize > screenSize.x)
      m_center.x = screenSize.x - shipSize;
    if(m_center.y + shipSize > screenSize.y)
      m_center.y = screenSize.y - shipSize;
    if(m_center.x - shipSize < 0)
      m_center.x = 0 + shipSize;
    if(m_center.y - shipSize < 0)
      m_center.y = 0 + shipSize;

    m_ai->Do();

    if (Length(m_speed) > DiskShip::maxSpeed)
    {
      m_speed = Normalize(m_speed, DiskShip::maxSpeed);
    }
  }
}

void DiskShip::Shoot(const Coordinate& target)
{
  if (0 == m_cooldown)
  {
    if (++m_bulletNum == DiskShip::bulletLimit)
    {
      m_cooldown = cooldown;
      m_bulletNum = 0;
    }

    Coordinate targetvector = target - m_center;
    Coordinate offset = targetvector * laserLength / (Length(targetvector) - shipSize - 1);
    Coordinate begin = (targetvector * laserLength / Length(targetvector)) + m_center + offset;
    Coordinate end = m_center + offset;
    m_frame.AddProjectile(new PulseLaser(begin, end, m_laserColor, m_frame, GetTeam()));
  }
}

Coordinate DiskShip::GetCenter() const
{
  return m_center;
}

Coordinate::CoordType DiskShip::GetSize() const
{
  return shipSize;
}

void DiskShip::Hit()
{
  m_speed = Coordinate();
  m_dead = deadInterval;
  m_explode = explosionInterval;
}

bool DiskShip::Alive()
{
  return 0 == m_dead;
}

//
// DiskShipAiRandom functions
//

void DiskShipAiRandom::Do()
{
  //random movement
  if (!(GetTicker() % changeDirectionInterval))
  {
    m_randum = Normalize(Coordinate(Random(1000) - 500, Random(1000) - 500), DiskShip::maxSpeed);
  }
  GetSpeed() += m_randum;

  //shooting if feasible
  Wiz::ShipTravel enemies = GetEnemies();
  if (!enemies.empty())
  {
    const Hitable* enemy = 0;
    if (!m_target || (!(GetTicker() % changeTargetInterval)))
    {
      enemy = enemies[Random(enemies.size())];
    }
    //found enemy. so shoot
    if (enemy)
    {
      Shoot(enemy->GetCenter());
    }
  }

}

//
// DiskShipAiRanger functions
//

void DiskShipAiRanger::Do()
{
  Wiz::ShipTravel enemies = GetEnemies();

  if (!enemies.empty())
  {
    const Hitable* enemy = FindClosest(enemies, GetCenter());

    //found enemy. so shoot
    Coordinate miss = Rotate90Cw(Normalize(enemy->GetCenter() - GetCenter(), missFactor));
    miss = (miss * Random(100)) / 50 - miss;
    Shoot(enemy->GetCenter() + miss);

    //and move
    int minDistance = minDistanceRatio * std::min(GetSize().x, GetSize().y);
    Coordinate modVector = GetCenter() - enemy->GetCenter();
    //if far enough, we move sideways to make it harder to hit
    if (Length(modVector) > minDistance)
    {
      modVector = Rotate90Cw(modVector);
    }

    if (std::abs(modVector.x) + std::abs(modVector.y) > 1)
    {
      modVector = Normalize(modVector, DiskShip::maxSpeed);
      GetSpeed() += modVector;
    }
  }
}

//
// PulseLaser functions
//

void PulseLaser::Draw()
{
  DrawLine(m_front, m_back, m_color);
}

void PulseLaser::Move()
{
  m_front += m_speed;

  if (m_frame.CheckCollision(m_front, m_back, GetTeam()))
  {
    m_frame.RemoveProjectile(this);
  }

  m_back += m_speed;

  Size screenSize = GetSize();

  if (m_back.x > screenSize.x || m_back.x < 0 || m_back.y > screenSize.y || m_back.y < 0)
  {
    m_frame.RemoveProjectile(this);
  }
}

const Hitable* FindClosest(const Wiz::ShipTravel& list, Hitable* me)
{
  return *min_element(list.begin(), list.end(), DistanceComparer(me));
}

const Hitable* FindClosest(const Wiz::ShipTravel& list, const Coordinate& center)
{
  return *min_element(list.begin(), list.end(), DistanceComparer(center));
}

void RemoveMe(Wiz::ShipTravel& list, Hitable* me)
{
  Wiz::ShipTravel::iterator iter = find(list.begin(), list.end(), me);
  if (list.end() != iter)
  {
    list.erase(iter);
  }
}


int DiskShip::shipSize                        = 5;
int DiskShip::maxSpeed                        = 5;
int DiskShip::bulletLimit                     = 7;
int DiskShip::cooldown                        = 40;
int DiskShip::laserLength                     = 13;
int DiskShip::deadInterval                    = 25;
int DiskShip::explosionInterval               = 8;
int DiskShip::explosionSize                   = 10;

int DiskShipAiRandom::changeDirectionInterval = 10;
int DiskShipAiRandom::changeTargetInterval    = 3;

double DiskShipAiRanger::minDistanceRatio     = 0.2;
int DiskShipAiRanger::maxDistance             = 450;
int DiskShipAiRanger::missFactor              = 30;

int PulseLaser::speed                         = 15;

