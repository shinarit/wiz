#include "flyerz.hpp"
#include "wiz.hpp"

void DiskShip::Draw()
{
  DrawCircle(m_center, shipSize, m_color, true);
}

void DiskShip::Move()
{
  Size screenSize = GetSize();

  m_center.x = (m_center.x + m_speed.x) % screenSize.x;
  m_center.y = (m_center.y + m_speed.y) % screenSize.y;

  if(m_center.x + shipSize > screenSize.x)
    m_center.x = screenSize.x - shipSize;
  if(m_center.y + shipSize > screenSize.y)
    m_center.y = screenSize.y - shipSize;
  if(m_center.x - shipSize < 0)
    m_center.x = 0 + shipSize;
  if(m_center.y - shipSize < 0)
    m_center.y = 0 + shipSize;

  if (0 == ((++m_ticker) % cooldownInterval))
  {
    if (m_bulletNum > 0)
    {
      --m_bulletNum;
    }
    Shoot();
  }
}

void DiskShip::Shoot()
{
  if (m_bulletNum < DiskShip::bulletLimit)
  {
    ++m_bulletNum;
    m_frame.AddProjectile(new PulseLaser(Coordinate(m_center.x - 50, m_center.y - 50), m_center, Colors::green, m_frame));
  }
}

void PulseLaser::Draw()
{
  DrawLine(m_front, m_back, m_color);
}

void PulseLaser::Move()
{
  m_front += m_speed;
  //collision check
  m_back += m_speed;

  Size screenSize = GetSize();

  if (m_back.x > screenSize.x || m_back.x < 0 || m_back.y > screenSize.y || m_back.y < 0)
  {
    m_frame.RemoveProjectile(this);
  }
}



int DiskShip::shipSize          = 7;
int DiskShip::bulletLimit       = 7;
int DiskShip::cooldownInterval  = 4;
int PulseLaser::pulseLaserSpeed = 15;

