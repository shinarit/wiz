#ifndef EVENTS_H
#define EVENTS_H

#include "drawinterface.hpp"

class Event
{
  public:
    enum EventMask
    {
      ShipMove = 1 << 0, ShipShoot = 1 << 1, ShipTurnBegin = 1 << 2, ShipDie = 1 << 3, ShipSpawn = 1 << 4
    };

  private:
    struct SimpleCoordinate
    {
      Coordinate::CoordType x;
      Coordinate::CoordType y;
    };

    struct ShipMoveData
    {
      SimpleCoordinate from;
      SimpleCoordinate to;
    };
    struct ShipShootData
    {
      SimpleCoordinate target;
    };

    union EventData
    {
      ShipMoveData shipMoveData;
      ShipShootData shipShootData;
    };
};

#endif // EVENTS_H
