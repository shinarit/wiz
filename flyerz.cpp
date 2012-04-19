#include "flyerz.hpp"
#include "wiz.hpp"

#include <algorithm>
#include <cmath>

const Hitable* FindClosest(const Wiz::ShipTravel& list, Hitable* me);
void RemoveMe(Wiz::ShipTravel& list, Hitable* me);

struct DistanceComparer
{
  DistanceComparer(Hitable* toCompare): m_comp(toCompare->GetCenter())
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

DiskShip::DiskShip(Coordinate center, Color color, Wiz& frame, int team): Hitable(team, frame), m_center(center), m_color(color), m_bulletNum(0), m_dead(0), m_ai(new DiskShipAi(this))
{}

void DiskShip::Draw()
{
  if (!m_dead)
  {
    DrawCircle(m_center, shipSize, m_color, true);
  }
}

void DiskShip::Move()
{
  if (0 == ((++m_ticker) % cooldownInterval))
  {
    if (m_bulletNum > 0)
    {
      --m_bulletNum;
    }
  }

  if (!m_dead)
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
  }
  else
  {
    if(!(--m_dead))
    {
      m_center = m_frame.PlaceMe(GetTeam());
    }
  }
}

void DiskShip::Shoot(const Coordinate& target)
{
  if (m_bulletNum < DiskShip::bulletLimit)
  {
    ++m_bulletNum;
    Coordinate targetvector = target - m_center;
    Coordinate offset = targetvector * laserLength / (Length(targetvector) - shipSize - 1);
    Coordinate begin = (targetvector * laserLength / Length(targetvector)) + m_center + offset;
    Coordinate end = m_center + offset;
    m_frame.AddProjectile(new PulseLaser(begin, end, Colors::green, m_frame, GetTeam()));
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
  m_dead = deadInterval;
}

bool DiskShip::Alive()
{
  return 0 == m_dead;
}

//
// DiskShipAi functions
//

#include <iostream>

void DiskShipAi::Do()
{
  Wiz::ShipTravel enemies = m_ship->m_frame.GetEnemies(m_ship->GetTeam());
  RemoveMe(enemies, m_ship);

  if (!enemies.empty())
  {
    const Hitable* enemy = FindClosest(enemies, m_ship);

    //found enemy. so shoot
    m_ship->Shoot(enemy->GetCenter());

    std::cout << "orig speed " << m_ship->m_speed.x << ':' << m_ship->m_speed.y << '\n';
    //and move
    Coordinate modVector;
    int distance = Distance(enemy->GetCenter(), m_ship->m_center);
    std::cout << "distance " << distance << '\n';
    if (distance < minDistance)
    {
      modVector = enemy->GetCenter() - m_ship->m_center;
    }
    else if (distance > maxDistance)
    {
      modVector = m_ship->m_center - enemy->GetCenter();
    }
    std::cout << "modvector " << modVector.x << ':' << modVector.y << '\n';
    if (std::abs(modVector.x * modVector.y) > 1)
    {
      m_ship->m_speed += modVector;
      std::cout << "modified speed " << m_ship->m_speed.x << ':' << m_ship->m_speed.y << '\n';
      m_ship->m_speed = m_ship->m_speed * DiskShip::maxSpeed / Length(m_ship->m_speed);
      std::cout << "normalized speed " << m_ship->m_speed.x << ':' << m_ship->m_speed.y << '\n';
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

void RemoveMe(Wiz::ShipTravel& list, Hitable* me)
{
  Wiz::ShipTravel::iterator iter = find(list.begin(), list.end(), me);
  if (list.end() != iter)
  {
    list.erase(iter);
  }
}


int DiskShip::shipSize          = 7;
int DiskShip::maxSpeed          = 5;
int DiskShip::bulletLimit       = 1;
int DiskShip::cooldownInterval  = 4;
int DiskShip::laserLength       = 25;
int DiskShip::deadInterval      = 25;

int DiskShipAi::minDistance     = 50;
int DiskShipAi::maxDistance     = 150;

int PulseLaser::speed           = 15;

