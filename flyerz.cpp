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
#include "ais.hpp"

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

DiskShip::DiskShip(Coordinate center,Color color, Color lasercolor, const std::string &name, int id, Wiz &frame, int team): Hitable(name, team, frame), m_center(center), m_shipColor(color), m_laserColor(lasercolor), m_id(id), m_bulletNum(0), m_cooldown(0), m_dead(0), m_textAlign(-1), m_ai(0)
{ }

const Color ExplosionColors[] = {Colors::yellow, Colors::orange};

void DiskShip::Draw()
{
  if (m_dead)
  {
    if (m_explode)
    {
      if (2 != DrawWrapper::Random(3))   //67% of showing some explosion
      {
        Color color = ExplosionColors[DrawWrapper::Random(2)];
        int size = explosionSize / 2 + DrawWrapper::Random(explosionSize / 2) * (m_explode / 2);

        DrawWrapper::DrawCircle(m_center, size, color, true);

        --m_explode;
      }
    }
  }
  else
  {
    DrawWrapper::DrawCircle(m_center, shipSize, m_shipColor, true);
    m_textAlign = DrawWrapper::DrawTextCentered(GetName(), Coordinate(m_center.x, m_center.y - shipSize), m_shipColor, m_textAlign);
  }
}

void DiskShip::PrepareMove()
{
  m_shot = false;
  m_shoot = false;
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
    m_ai->Do();
  }
}

void DiskShip::ExecuteMove()
{
  if (!m_dead)
  {
    if (Length(m_speed) > DiskShip::maxSpeed)
    {
      m_speed = Normalize(m_speed, DiskShip::maxSpeed);
    }

    Size screenSize = ::DrawWrapper::GetSize();

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

    if (m_shoot)
    {
      Coordinate targetvector = m_target - m_center;
      Coordinate step = Normalize(targetvector, 1.0);
      Coordinate end = m_center + step * (shipSize + 1);
      Coordinate begin = end + step * laserLength;
      m_frame.AddProjectile(new PulseLaser(begin, end, m_laserColor, m_frame, GetTeam(), m_id));
    }
  }
}

void DiskShip::Shoot(const Coordinate& target)
{
  if (!m_shot && 0 == m_cooldown)
  {
    if (++m_bulletNum == DiskShip::bulletLimit)
    {
      m_cooldown = cooldown;
      m_bulletNum = 0;
    }

    m_shoot = true;
    m_target = target;

    m_shot = true;
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
  m_cooldown = 0;
  m_bulletNum = 0;
}

bool DiskShip::Alive() const
{
  return 0 == m_dead;
}



//
// PulseLaser functions
//

void PulseLaser::Draw()
{
  DrawWrapper::DrawLine(m_front, m_back, m_color);
}

void PulseLaser::PrepareMove()
{
  m_front += m_speed;

  if (m_frame.CheckCollision(m_front, m_back, GetTeam(), GetOwner()))
  {
    m_frame.RemoveProjectile(this);
  }

  m_back += m_speed;

  Size screenSize = DrawWrapper::GetSize();

  if (m_back.x > screenSize.x || m_back.x < 0 || m_back.y > screenSize.y || m_back.y < 0)
  {
    m_frame.RemoveProjectile(this);
  }
}

void PulseLaser::ExecuteMove()
{
}

CollisionDescriptor PulseLaser::GetCollision() const
{
  return std::make_pair(m_front, m_back);
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

int PulseLaser::speed                         = 15;
