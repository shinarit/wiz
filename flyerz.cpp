#include "flyerz.hpp"

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
}

void DiskShip::Shoot()
{
  if (m_bulletNum < DiskShip::bulletLimit)
  {
    ++m_bulletNum;
    //add projectile
  }
}

void PulseLaser::Draw()
{
  DrawLine(m_front, m_back, m_color);
}
#include <iostream>

void PulseLaser::Move()
{
  std::cout << m_front.x << ':' << m_front.y << '\n';
  std::cout << m_back.x << ':' << m_back.y << '\n';
  std::cout << m_speed.x << ':' << m_speed.y << '\n';

  m_front += m_speed;
  //collision check
  m_back += m_speed;
}



int DiskShip::shipSize          = 7;
int DiskShip::bulletLimit       = 7;
int PulseLaser::pulseLaserSpeed = 15;

