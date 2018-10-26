#include "Snake.h"

using std::cout;
using std::cerr;

Snake::Snake(int player_number)
 : m_body()
{
  m_score = 0;
  m_direction = UP;
  m_player_number = player_number;
}

void Snake::initPosition(Coords coord)
{
  m_body.push_front(coord);
}

void Snake::setDirection(Direction direction)
{
  m_direction = direction;
}

Direction Snake::getDirection()
{
  return m_direction;
}

Coords Snake::getHeadPosition()
{
  Coords headPosition = *m_body.begin();
  return headPosition;
}

void Snake::addPart(Coords coords)
{
  m_body.push_front(coords);
}

int Snake::getScore()
{
  return m_score;
}

void Snake::addScore(int score)
{
  m_score += score;
}

void Snake::print()
{
  for(auto it = m_body.begin() ; it != m_body.end() ; ++it)
  {
    std::cout << "pos = " << it->x << " , " << it->y << std::endl;
  }
}

Coords Snake::removePart()
{
  Coords lastPosition = m_body.back();
  m_body.pop_back();
  return lastPosition;
}

