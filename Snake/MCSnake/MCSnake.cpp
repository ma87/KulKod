#include "MCSnake.h"

SnakeMC::SnakeMC()
 :Snake()
{

}

SnakeMC::~SnakeMC()
{

};

Coords getAppleCoords(const Block ** map, int nrows, int ncols)
{
  for(unsigned int i=0 ; i<nrows ; i++)
  {
    for (unsigned int j=0; j<ncols ; j++)
    {
      if (map[i][j] == APPLE)
      {
        return (Coords){i, j};
      }
    }
  }
}

void SnakeMC::updateDirection(const Block ** map, int nrows, int ncols)
{
  Coords apple = getAppleCoords(map, nrows, ncols);

  Coords head = getHeadPosition();
  switch(m_direction)
  {
    case UP:
      if (head.x == apple.x)
      {
        m_direction = (head.y < apple.y) ? RIGHT : LEFT;
      }
      else if (head.x > apple.x)
      {
        m_direction = UP;
      }
      else if (head.x < apple.x)
      {
        if (map[head.x][head.y + 1] == WALL)
        {
          m_direction = LEFT;
        }
        else
        {
          m_direction = RIGHT;
        }
      }
    break;
    case DOWN:
      if (head.x == apple.x)
      {
        m_direction = (head.y < apple.y) ? RIGHT : LEFT;
      }
      else if (head.x > apple.x)
      {
        if (map[head.x][head.y + 1] == WALL)
        {
          m_direction = LEFT;
        }
        else
        {
          m_direction = RIGHT;
        }
      }
      else if (head.x < apple.x)
      {
        m_direction = DOWN;
      }
    break;
    case LEFT:
      if (head.y == apple.y)
      {
        m_direction = (head.x < apple.x) ? DOWN : UP;
      }
      else if (head.y > apple.y)
      {
        m_direction = LEFT;
      }
      else if (head.y < apple.y)
      {
        if (map[head.x + 1][head.y] == WALL)
        {
          m_direction = UP;
        }
        else
        {
          m_direction = DOWN;
        }
      }
    break;
    case RIGHT:
      if (head.y == apple.y)
      {
        m_direction = (head.x < apple.x) ? DOWN : UP;
      }
      else if (head.y < apple.y)
      {
        m_direction = RIGHT;
      }
      else if (head.y > apple.y)
      {
        if (map[head.x + 1][head.y] == WALL)
        {
          m_direction = UP;
        }
        else
        {
          m_direction = DOWN;
        }
      }
    break;
  }
}

std::string SnakeMC::getName()
{
  return "SnakeMC";
}
