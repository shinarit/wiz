//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// flyerz.hpp
//
// anything that can be animated or collided with
//

#ifndef FLYERZ_H
#define FLYERZ_H

#include <string>

#include "drawinterface.hpp"
#include "wiz.hpp"


const Hitable* FindClosest(const Wiz::ShipTravel& list, Hitable* me);
const Hitable* FindClosest(const Wiz::ShipTravel& list, const Coordinate& center);
void RemoveMe(Wiz::ShipTravel& list, Hitable* me);

class Flyer
{
  public:
    Flyer(int team, Wiz& frame): m_frame(frame), m_ticker(0), m_team(team)
    {}
    virtual ~Flyer()
    {}
    virtual void Draw() = 0;
    virtual void Move() = 0;

    int GetTeam() const
    {
      return m_team;
    }

  protected:
    Wiz&        m_frame;
    int         m_ticker;

  private:
    const int   m_team;
};

class Hitable: public Flyer
{
  public:
    Hitable(const std::string& name, int team, Wiz& frame): Flyer(team, frame), m_name(name)
    {}
    virtual Coordinate GetCenter() const = 0;
    virtual Coordinate::CoordType GetSize() const = 0;
    virtual void Hit() = 0;
    virtual bool Alive() const = 0;

    const std::string& GetName() const
    {
      return m_name;
    }

  protected:
    std::string m_name;
};

class Owned: public Flyer
{
  public:
    Owned(int owner, Wiz& frame, int team = 0): Flyer(team, frame), m_owner(owner)
    { }
    int GetOwner() const
    {
      return m_owner;
    }
    void SetOwner(int owner)
    {
      m_owner = owner;
    }

  private:
    int m_owner;
};

class DiskShipAi;

class DiskShip: public Hitable
{
  public:
    static int shipSize;
    static int maxSpeed;
    static int bulletLimit;
    static int cooldown;
    static int laserLength;
    static int deadInterval;
    static int explosionInterval;
    static int explosionSize;

    DiskShip(Coordinate center, Color shipColor, Color laserColor, const std::string& name, int id, Wiz& frame, int team = 0);
    DiskShipAi* SetAi(DiskShipAi* ai)
    {
      DiskShipAi* old = m_ai;
      m_ai = ai;
      return old;
    }

    //from Flyer
    virtual void Draw();
    virtual void Move();
    //from Hitable
    virtual Coordinate GetCenter() const;
    virtual Coordinate::CoordType GetSize() const;
    virtual void Hit();
    virtual bool Alive() const;

  private:
    friend class DiskShipAi;

    void Shoot(const Coordinate& target);

    Coordinate  m_center;
    Color       m_shipColor;
    Color       m_laserColor;
    Coordinate  m_speed;

    int         m_id;
    int         m_bulletNum;
    int         m_cooldown;
    int         m_dead;
    int         m_explode;

    int         m_textAlign;

    DiskShipAi* m_ai;
};

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

class PulseLaser: public Owned
{
  public:
    static int speed;

    PulseLaser(Coordinate begin, Coordinate end, Color color, Wiz& frame, int team, int id): Owned(id, frame, team), m_front(begin), m_back(end), m_speed((m_front - m_back)), m_color(color)
    {
      m_speed = (m_speed * speed) / Length(m_speed);
    }
    //from Flyer
    virtual void Draw();
    virtual void Move();

  private:
    Coordinate  m_front;
    Coordinate  m_back;
    Coordinate  m_speed;
    Color       m_color;
};


#endif // FLYERZ_H
