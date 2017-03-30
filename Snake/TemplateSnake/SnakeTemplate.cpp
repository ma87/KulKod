#include "SnakeTemplate.h"

SnakeTemplate::SnakeTemplate()
  :Snake()
{
}

SnakeTemplate::~SnakeTemplate()
{


};

void SnakeTemplate::updateDirection(const Block ** map, int nrows, int ncols)
{
  // update member m_direction considering the current map
}

std::string SnakeTemplate::getName()
{
  return "SnakeTemplate";
}
