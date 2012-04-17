#ifndef WIZ_HPP
#define WIZ_HPP

#include <vector>

class Flyer;
class Coordinate;

class Wiz
{
  public:
    Wiz();
    ~Wiz();
    void DrawFrame();
    bool CheckCollision(const Coordinate& begin, const Coordinate& end);
    void AddProjectile(Flyer*);
    void RemoveProjectile(Flyer*);

  private:
    void MoveAll();
    void Clean();
    void KillProjectile(Flyer*);

    typedef std::vector<Flyer*> ShipList;
    ShipList ships;

    typedef std::vector<Flyer*> ProjectileList;
    ProjectileList projectiles;

    ProjectileList deads;
};

#endif // WIZ_HPP
