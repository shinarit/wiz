//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// ais.cpp
//
// artifical intelligence code, including the endpoint of remote control
//

#include "ais.hpp"
#include "flyerz.hpp"

#include <sstream>

//
// DiskShipAiRandom functions
//

void DiskShipAiRandom::Do()
{
  //random movement
  if (!(GetTicker() % changeDirectionInterval))
  {
    m_randum = Normalize(Coordinate(DrawWrapper::Random(1000) - 500, DrawWrapper::Random(1000) - 500), DiskShip::maxSpeed);
  }
  GetSpeed() += m_randum;

  //shooting if feasible
  Wiz::ShipTravel enemies = GetEnemies();
  if (!enemies.empty())
  {
    const Hitable* enemy = enemies[DrawWrapper::Random(enemies.size())];
    Shoot(enemy->GetCenter());
  }
}

//
// DiskShipAiRanger functions
//

double abs(double num)
{
  return ((num > 0)? (num) : (-num));
}

void DiskShipAiRanger::Do()
{
  Wiz::ShipTravel enemies = GetEnemies();

  if (!enemies.empty())
  {
    const Hitable* enemy = FindClosest(enemies, GetCenter());

    //found enemy. so shoot
    Coordinate targetVector = GetCenter() - enemy->GetCenter();
    Coordinate::CoordType distance = Length(targetVector);
    Coordinate miss = Rotate90Cw(Normalize(targetVector, missFactor));
    miss = ((miss * DrawWrapper::Random(100)) / 50 - miss) * distance / 100;
    Shoot(enemy->GetCenter() + miss);

    //and move
    Coordinate::CoordType minDistance = minDistanceRatio * std::min(DrawWrapper::GetSize().x, DrawWrapper::GetSize().y);
    //if far enough, we move sideways to make it harder to hit
    if (distance > minDistance)
    {
      if (distance > minDistance + maxDistance)
      {
        targetVector = -targetVector;
      }

      Coordinate evadeVector = Rotate90Cw(targetVector);
      targetVector += evadeVector;
    }

    if (abs(targetVector.x) + abs(targetVector.y) > 1)
    {
      targetVector = Normalize(targetVector, DiskShip::maxSpeed);
      GetSpeed() += targetVector;
    }
  }
}

//
// DiskShipAiTest functions
//

void DiskShipAiTest::Do()
{
  Wiz::ShipTravel enemies = GetEnemies();
  if (!enemies.empty())
  {
    const Hitable* enemy = FindClosest(enemies, GetCenter());
    Shoot(enemy->GetCenter());
  }
  GetSpeed() = Coordinate();
}

//
// DiskShipAiRemote functions
//

void DiskShipAiRemote::Do()
{
  m_communication.Send(RemoteProtocol::BEGIN);
  std::string str = m_communication.Receive();
  std::istringstream istr(str);
  while (RemoteProtocol::END != str)
  {
    std::string response;
    istr >> str;
    if (RemoteProtocol::COMMAND_SPEED == str)
    {
      if (Alive())
      {
        double x, y;
        istr >> x >> y;
        GetSpeed() = Coordinate(x, y);

        response = RemoteProtocol::ACK;
      }
      else
      {
        response = RemoteProtocol::DEAD;
      }
    }
    else if (RemoteProtocol::COMMAND_SHOOT == str)
    {
      if (Alive())
      {
        double x, y;
        istr >> x >> y;
        Shoot(Coordinate(x, y));

        response = RemoteProtocol::ACK;
      }
      else
      {
        response = RemoteProtocol::DEAD;
      }
    }
    else if (RemoteProtocol::QUERY == str)
    {
      istr >> str;
      std::ostringstream ostr;
      if (RemoteProtocol::QUERY_POSITION == str)
      {
        Coordinate coords = GetCenter();
        ostr << RemoteProtocol::RESPONSE_POSITION << ' ' << coords.x << ' ' << coords.y;
      }
      else if (RemoteProtocol::QUERY_SPEED == str)
      {
        ostr << RemoteProtocol::RESPONSE_SPEED << ' ' << GetSpeed().x << ' ' << GetSpeed().y;
      }
      else if (RemoteProtocol::QUERY_TEAM == str)
      {
        ostr << RemoteProtocol::RESPONSE_TEAM << ' ' << GetTeam();
      }
      else if (RemoteProtocol::QUERY_ENEMIES == str)
      {
        ostr << RemoteProtocol::RESPONSE_ENEMIES << ' ';
        Wiz::ShipTravel enemies = GetEnemies();
        for (Wiz::ShipTravel::iterator it = enemies.begin(); enemies.end() != it; ++it)
        {
          const Hitable& enemy = *(*it);
          Coordinate center = enemy.GetCenter();
          ostr << enemy.GetTeam() << ' ' << center.x << ' ' << center.y << ' ';
        }
      }
      else if (RemoteProtocol::QUERY_FRIENDS == str)
      {
        ostr << RemoteProtocol::RESPONSE_FRIENDS << ' ';
        Wiz::ShipTravel team = GetTeammates();
        for (Wiz::ShipTravel::iterator it = team.begin(); team.end() != it; ++it)
        {
          Coordinate center = (*it)->GetCenter();
          ostr << center.x << ' ' << center.y << ' ';
        }
      }
      else if (RemoteProtocol::QUERY_BULLETS == str)
      {
        ostr << RemoteProtocol::RESPONSE_BULLETS << ' ';
        Wiz::LaserList bullets = GetBullets();
        for (Wiz::LaserList::iterator it = bullets.begin(); bullets.end() != it; ++it)
        {
          ostr << it->first << ' ' << it->second.first.x << ' ' << it->second.first.y << ' ' << it->second.second.x << ' ' << it->second.second.y << ' ';
        }
      }
      else if (RemoteProtocol::QUERY_CONTEXT == str)
      {
        Size size = DrawWrapper::GetSize();
        ostr << "context " << size.x << ' ' << size.y << ' ' << DiskShip::shipSize << ' ' << DiskShip::maxSpeed << ' ' << DiskShip::bulletLimit << ' ' <<
                DiskShip::cooldown << ' ' << DiskShip::laserLength << ' ' << PulseLaser::speed << ' ' << DiskShip::deadInterval;
      }
      response = ostr.str();
    }
    else
    {
      response = RemoteProtocol::ACK;
    }

    m_communication.Send(response);

    str = m_communication.Receive();
    istr.clear();
    istr.str(str);
  }
}

int DiskShipAiRandom::changeDirectionInterval = 10;
int DiskShipAiRandom::changeTargetInterval    = 3;

double DiskShipAiRanger::minDistanceRatio     = 0.2;
int DiskShipAiRanger::maxDistance             = 300;
int DiskShipAiRanger::missFactor              = 30;
