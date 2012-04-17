#ifndef WIZ_HPP
#define WIZ_HPP

#include <vector>

class Flyer;
class Hitable;
class Coordinate;

class Wiz
{
  public:
    Wiz();
    ~Wiz();
    void DrawFrame();
    bool CheckCollision(const Coordinate& begin, const Coordinate& end, int team = 0);
    void AddProjectile(Flyer*);
    void RemoveProjectile(Flyer*);
    Coordinate PlaceMe(int team);

  private:
    void MoveAll();
    void Clean();
    void KillProjectile(Flyer*);

    typedef std::vector<Hitable*> ShipList;
    ShipList ships;

    typedef std::vector<Flyer*> ProjectileList;
    ProjectileList projectiles;

    ProjectileList deads;
};

#endif // WIZ_HPP
