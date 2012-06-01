//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// wiz.hpp
//
// the main class that represents and moves the draw logic
//
// remake of the great blaster screensaver, with more features i missed when watching it
// list of planned features:
//                            teams (so that red and green actually means something)
//                            score
//                            better(?) AI
//
//

#ifndef WIZ_HPP
#define WIZ_HPP

#include <vector>

class Options;
class Owned;
class Hitable;
class Coordinate;

class Wiz
{
  public:
    Wiz();
    ~Wiz();
    void Init(const Options &options);
    void DrawFrame();
    bool CheckCollision(const Coordinate& begin, const Coordinate& end, int team, int owner) const;
    void AddProjectile(Owned*);
    void RemoveProjectile(Owned*);
    Coordinate PlaceMe(int team) const;

    typedef std::vector<const Hitable*> ShipTravel;
    ShipTravel GetEnemies(int team) const;
    ShipTravel GetTeam(int team) const;

    //(team, (front, back))
    typedef std::vector<std::pair<int, std::pair<Coordinate, Coordinate> > > LaserList;
    LaserList GetBullets() const;

  private:
    static const int CheckDistance = 4;
    static int Margin;

    typedef std::vector<Hitable*> ShipList;
    ShipList ships;
    typedef std::vector<Owned*> ProjectileList;
    ProjectileList projectiles;
    typedef std::vector<int> ScoreList;
    mutable ScoreList scores;

    ProjectileList deads;

    mutable LaserList projectileCache;

    void MoveAll();
    void Clean();
    void DrawScore();
    void KillProjectile(Owned*);
    ShipList GetPotentials(int team, Coordinate center, int dist) const;
};

#endif // WIZ_HPP
