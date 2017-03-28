#include "GameController.h"

GameController::GameController(int nrows, int ncols)
   : m_map(nrows, ncols),
   m_aiManager(),
   m_snakes()
{
}

GameController::~GameController()
{

}



void GameController::start()
{
  if (m_map.createApple())
  {
    for(std::vector<Snake *>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
    {
      Coords startPosition = m_map.initSnakePosition((*snake)->m_player_number);
      (*snake)->initPosition(startPosition);
    }
  }
}

void GameController::end(int crash_player_number)
{
  FILE * f_result;
  if (m_snakes.size() > 1)
  {
    f_result = fopen("matches.txt", "a");
    for(std::vector<Snake *>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
    {
      fprintf(f_result, "%s\t%d\t", (*snake)->getName().c_str(), (*snake)->getScore());
    }
    fprintf(f_result, "%s\n", m_snakes[crash_player_number-1]->getName().c_str());
    std::cout << "player " << crash_player_number << " crash" << std::endl;
  }
  else
  {
    f_result = fopen("scores.txt", "a");
    fprintf(f_result, "%s\t%d\n", m_snakes[0]->getName().c_str(), m_snakes[0]->getScore());
  }

  fclose(f_result);
}

int GameController::update()
{
  return update(1);
}

int GameController::update(int nsamples)
{
  int snake_crash = 0;
  for(int i=0 ; i<nsamples ; i++)
  {
    for(std::vector<Snake *>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
    {
      (*snake)->updateDirection((const Block **) m_map.getMap(), m_map.getSize().x, m_map.getSize().y);

      Result res = updateMap(*snake);
      if (res == SNAKE_CRASH)
      {
        snake_crash = (*snake)->m_player_number;
      }

      std::cout << (*snake)->getName() << " score = " << (*snake)->getScore() << std::endl;
    }
  }

  m_map.print();

  return snake_crash;
}

Result GameController::updateMap(Snake * snake)
{
  Result res;
  Coords head = snake->getHeadPosition();

  Coords nextPosition;

  switch(snake->getDirection())
  {
    case UP:
      nextPosition = (Coords) {head.x - 1, head.y};
    break;
    case DOWN:
      nextPosition = (Coords) {head.x + 1, head.y};
    break;
    case LEFT:
      nextPosition = (Coords) {head.x, head.y - 1};
    break;
    case RIGHT:
      nextPosition = (Coords) {head.x, head.y + 1};
    break;
  }

  Block nextBlockType = (Block)(m_map.getBlock(nextPosition.x, nextPosition.y) & 0x0F);
  if (nextBlockType == WALL || nextBlockType == SNAKE)
  {
    res = SNAKE_CRASH;
  }
  else if (nextBlockType == APPLE)
  {

    snake->addPart(nextPosition);
    res = APPLE_EATED;
    m_map.setBlock(nextPosition.x, nextPosition.y, snake->m_player_number);
    snake->addScore(1);

    m_map.createApple();
  }
  else
  {
    snake->addPart(nextPosition);
    m_map.setBlock(nextPosition.x, nextPosition.y, snake->m_player_number);

    Coords lastPos = snake->removePart();
    m_map.setBlock(lastPos.x, lastPos.y, VOID);
    res = NOTHING;
  }

  return res;
}

void GameController::loadSnake(char * dllName, short unsigned int player_number)
{
  Snake * new_snake = (Snake *)m_aiManager.getAI(dllName);
  new_snake->m_player_number = player_number;
  m_snakes.push_back(new_snake);

}
