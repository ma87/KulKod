#ifndef __SNAKE__EXAMPLE_H
#define __SNAKE__EXAMPLE_H

#include "Snake.h"

class SnakeExample : public Snake
{
  public:
   SnakeExample(std::string name);

   std::string getName();
   ~SnakeExample();
  
  private:
    std::string m_name;
};

#endif
