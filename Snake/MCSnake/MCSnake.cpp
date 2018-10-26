#include "MCSnake.h"

SnakeMC::SnakeMC(int player_number)
{
  m_player_number = player_number;
  m_headPosition = (Coords){0,0};
  m_direction = LEFT;
}

SnakeMC::~SnakeMC()
{

};

Direction SnakeMC::getDirection()
{
  return m_direction;
}

void SnakeMC::initHeadPosition(const char * map, int nrows, int ncols)
{
  int current_index = 0;
  for(unsigned int i=0 ; i<nrows ; i++)
  {
    for(unsigned int j=0 ; j<ncols ; j++)
    {
      if (map[current_index] & SNAKE)
      {
          unsigned int player_number = map[current_index] >> 4;
          if (player_number == m_player_number)
          {
            m_headPosition = (Coords){i,j};
            return;
          }
      }
      current_index++;
    }
  }
}

Coords SnakeMC::getAppleCoords(const char * map, int nrows, int ncols)
{
  int current_index = 0;
  for(unsigned int i=0 ; i<nrows ; i++)
  {
    for (unsigned int j=0; j<ncols ; j++)
    {
      if (map[current_index] == APPLE)
      {
        return (Coords){i, j};
      }
      current_index++;
    }
  }
  return (Coords){0, 0};
}

Block SnakeMC::getBlock(const char * map, int i, int j, int ncols)
{
  // Matrix in column order
  // Example 2x2
  // [map(0) map(1)]
  // [map(2) map(3)]
  return (Block)map[j + ncols * i];
}

void SnakeMC::updateDirection(const char * map, int nrows, int ncols)
{
  if ((m_headPosition.x == 0) && (m_headPosition.y == 0))
  {
    initHeadPosition(map, nrows, ncols);
  }

  Coords apple = getAppleCoords(map, nrows, ncols);
  Coords head = m_headPosition;
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
        if (getBlock(map, head.x, head.y + 1, ncols) == WALL)
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
        if (getBlock(map, head.x, head.y + 1, ncols) == WALL)
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
        if (getBlock(map, head.x + 1, head.y, ncols) == WALL)
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
        if (getBlock(map, head.x + 1, head.y, ncols) == WALL)
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

  // Update head position
  switch(m_direction)
  {
    case LEFT:
    m_headPosition.y -= 1;
    break;
    case RIGHT:
    m_headPosition.y += 1;
    break;
    case UP:
    m_headPosition.x -= 1;
    break;
    case DOWN:
    m_headPosition.x += 1;
    break;
  }
}

std::string SnakeMC::getName()
{
  return "SnakeMC";
}
