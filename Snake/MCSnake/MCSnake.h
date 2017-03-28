#ifndef __SNAKE__MC_H
#define __SNAKE__MC_H

#include "Snake.h"

class SnakeMC : public Snake
{
  public:
   SnakeMC();
   ~SnakeMC();
   
   std::string getName();
   void updateDirection(const Block ** block, int nrows, int ncols);
  
  private:
    std::string m_name;
};

extern "C" Snake* create() {
    return new SnakeMC();
}

extern "C" void destroy(Snake* p) {
    delete p;
}

#endif
