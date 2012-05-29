//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// randomai.cpp
//
// sample ai which copies the builtin random ai
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include "skeleton.hpp"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <commands file> <data file>\n";
    return 1;
  }

  std::ifstream in(argv[2]);
  std::ofstream out(argv[1]);

  if (!out || !in)
  {
    std::cerr << "Error with opening file\n";
    return 2;
  }

  Skeleton ipc(in, out);

  std::srand(std::time(0) + getpid());

  const int changeDirectionInterval = 10;
  const int changeTargetInterval = 3;
  const int maxSpeed = 5;

  Coordinate randum;
  int ticker = 0;
  std::string str;

  while (true)
  {
    ipc.Begin();

    if (!(ticker % changeDirectionInterval))
    {
      randum = Normalize(Coordinate(std::rand()%1000 - 500, std::rand()%1000 - 500), maxSpeed);
    }

    out << "get speed\n";
    Coordinate speed = ipc.GetSpeed();
    ipc.SetSpeed(speed + randum);
    //ipc.SetSpeed(Coordinate(10, 10));

    Skeleton::ShipList enemies = ipc.GetEnemies();
    if (!enemies.empty())
    {
      int index = std::rand() % enemies.size();
      std::pair<int, Coordinate> enemy = enemies[index];
      ipc.Shoot(enemy.second);
    }

    ipc.End();

    ++ticker;
  }
}
