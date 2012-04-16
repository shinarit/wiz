#ifndef WIZ_HPP
#define WIZ_HPP

#include <vector>

class Flyer;

class Wiz
{
  public:
    Wiz();
    ~Wiz();
    void DrawFrame();
    void AddProjectile(Flyer*);
    void RemoveProjectile(Flyer*);

  private:
    void MoveAll();

    typedef std::vector<Flyer*> ShipList;
    ShipList ships;

    typedef std::vector<Flyer*> ProjectileList;
    ProjectileList projectiles;
};

#endif // WIZ_HPP
