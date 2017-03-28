#include "SnakeExample.h"

SnakeExample::SnakeExample(std::string name)
  :Snake()
{
  m_name = name;  
}

SnakeExample::~SnakeExample()
{
  
  
};

std::string SnakeExample::getName()
{
  return m_name;
}