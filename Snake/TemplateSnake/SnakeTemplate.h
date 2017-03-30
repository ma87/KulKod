#ifndef __SNAKE__EXAMPLE_H
#define __SNAKE__EXAMPLE_H

#include "Snake.h"

class SnakeTemplate : public Snake
{
  public:
   SnakeTemplate();
   ~SnakeTemplate();

   std::string getName();
   void updateDirection(const Block ** block, int nrows, int ncols);
};

extern "C" Snake* create() {
    return new SnakeTemplate();
}

extern "C" void destroy(Snake* p) {
    delete p;
}

#endif
