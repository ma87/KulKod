#ifndef __SNAKE__H
#define __SNAKE__H

#include <iostream>

#include <list>
#include "Map.h"
#include "AI.h"

class Snake : AI
{
  public:
   Snake();
   virtual ~Snake() {}
   virtual std::string getName() = 0;
   std::string getType()
   {
     return "SNAKE";
   }

   virtual void updateDirection(const Block ** map, int nrows, int ncols) = 0;
   void initPosition(Coords startPosition);
   Direction getDirection();

   Coords getHeadPosition();
   void addPart(Coords pos);
   Coords removePart();
   void print();
   int getScore();
   void addScore(int score);
   short unsigned int m_player_number;

  private:
   std::list<Coords> m_body;
   int m_score;

  protected:
   Direction m_direction;
};


#endif
