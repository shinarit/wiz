#ifndef WIZ_HPP
#define WIZ_HPP

class Flyer;

class Wiz
{
  public:
    Wiz();
    ~Wiz();
    void DrawFrame();
  private:
    Flyer* ship;
};

#endif // WIZ_HPP
