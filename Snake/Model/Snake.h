#ifndef __SNAKE__H
#define __SNAKE__H

#include <iostream>

#include <list>
#include "Map.h"

class Snake
{
  public:
   Snake(int player_number);
   ~Snake() {}

   void setDirection(Direction direction);
   void initPosition(Coords startPosition);
   Direction getDirection();

   Coords getHeadPosition();
   void addPart(Coords pos);
   Coords removePart();
   void print();
   int getScore();
   void addScore(int score);

  private:
   std::list<Coords> m_body;
   int m_score;

  protected:
   Direction m_direction;

  public:
   int m_player_number;
};


#endif
