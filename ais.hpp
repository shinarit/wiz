//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// ais.hpp
//
// artifical intelligence code, including the endpoint of remote control
//

#ifndef AIS_H
#define AIS_H

#include "flyerz.hpp"

class DiskShipAi
{
  public:
    DiskShipAi(DiskShip* toLead): m_ship(toLead)
    {}
    virtual ~DiskShipAi()
    {}
    virtual void Do() = 0;

  protected:
    int GetTicker() const
    {
      return m_ship->m_ticker;
    }
    Coordinate& GetSpeed()
    {
      return m_ship->m_speed;
    }
    Coordinate GetCenter() const
    {
      return m_ship->m_center;
    }
    int GetTeam() const
    {
      return m_ship->GetTeam();
    }
    void Shoot(const Coordinate& target)
    {
      m_ship->Shoot(target);
    }

    Wiz::ShipTravel GetEnemies() const
    {
      Wiz::ShipTravel enemies = m_ship->m_frame.GetEnemies(m_ship->GetTeam());
      RemoveMe(enemies, m_ship);
      return enemies;
    }
    Wiz::ShipTravel GetTeammates() const
    {
      Wiz::ShipTravel mates = m_ship->m_frame.GetTeam(m_ship->GetTeam());
      RemoveMe(mates, m_ship);
      return mates;
    }
    Wiz::LaserList GetBullets() const
    {
      return m_ship->m_frame.GetBullets();
    }
    bool Alive() const
    {
      return m_ship->Alive();
    }
    void Kill()
    {
      m_ship->Hit();
    }

    DiskShip* m_ship;
};

class DiskShipAiRandom: public DiskShipAi
{
  public:
    static int changeDirectionInterval;
    static int changeTargetInterval;

    DiskShipAiRandom(DiskShip* toLead): DiskShipAi(toLead), m_randum(0, 0), m_target(0)
    {}
    virtual void Do();

  private:
    Coordinate      m_randum;
    const Hitable*  m_target;
};

class DiskShipAiRanger: public DiskShipAi
{
  public:
    static double minDistanceRatio;
    static int maxDistance;
    static int missFactor;

    DiskShipAiRanger(DiskShip* toLead): DiskShipAi(toLead)
    { }
    virtual void Do();
};

class DiskShipAiTeam: public DiskShipAi
{
  public:
    DiskShipAiTeam(DiskShip* toLead): DiskShipAi(toLead)
    { }
    virtual void Do();
};

class DiskShipAiTest: public DiskShipAi
{
  public:
    DiskShipAiTest(DiskShip* toLead): DiskShipAi(toLead)
    { }
    virtual void Do();
};

class DiskShipAiRemote: public DiskShipAi
{
  public:
    DiskShipAiRemote(DiskShip* toLead, Ipc ipc): DiskShipAi(toLead), m_communication(ipc)
    { }
    virtual void Do();
  private:
    Ipc m_communication;
};

#endif // AIS_H
