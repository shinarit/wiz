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
  private:
    void MoveAll();

    typedef std::vector<Flyer*> ShipList;
    ShipList ships;
};

#endif // WIZ_HPP
