#include "GameController.h"

GameController::GameController(int nrows, int ncols)
   : m_map(nrows, ncols),
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
    for(std::vector<Snake>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
    {
      Coords startPosition = m_map.initSnakePosition(snake->m_player_number);
      snake->initPosition(startPosition);
    }
  }
}

void GameController::createSnake(int player_number)
{
  m_snakes.push_back(player_number);
}

void GameController::end(int crash_player_number)
{
  FILE * f_result;
  if (m_snakes.size() > 1)
  {
    f_result = fopen("matches.txt", "a");
    for(std::vector<Snake>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
    {
      fprintf(f_result, "%d\t%d\t", snake->m_player_number, snake->getScore());
    }
    fprintf(f_result, "%d\n", crash_player_number);
    std::cout << "player " << crash_player_number << " crash" << std::endl;
  }
  else
  {
    f_result = fopen("scores.txt", "a");
    fprintf(f_result, "%d\n", m_snakes[0].getScore());
  }

  fclose(f_result);
}

int GameController::update(Direction * snake_direction)
{
  int snake_crash = 0;
  
  int current_snake = 0;
  for(std::vector<Snake>::iterator snake = m_snakes.begin() ; snake != m_snakes.end() ; ++snake)
  {
    snake->setDirection(snake_direction[current_snake]);
    Result res = updateMap(*snake);
    if (res == SNAKE_CRASH)
    {
      snake_crash = snake->m_player_number;
    }

    std::cout << "player " << current_snake + 1 << " score = " << snake->getScore() << std::endl;
    current_snake++;
  }
  
  m_map.print();

  return snake_crash;
}

int GameController::get_size_map()
{
  Coords size = m_map.getSize();
  int number_bytes_per_block = 1;
  return size.x * size.y * number_bytes_per_block;
}

int GameController::serialize_map(char * serialized_map, int size_buffer)
{
  return m_map.serialize(serialized_map, size_buffer);
}

Result GameController::updateMap(Snake & snake)
{
  Result res;
  Coords head = snake.getHeadPosition();

  Coords nextPosition;

  switch(snake.getDirection())
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

    snake.addPart(nextPosition);
    res = APPLE_EATED;
    m_map.setBlock(nextPosition.x, nextPosition.y, snake.m_player_number);
    snake.addScore(1);

    m_map.createApple();
  }
  else
  {
    snake.addPart(nextPosition);
    m_map.setBlock(nextPosition.x, nextPosition.y, snake.m_player_number);

    Coords lastPos = snake.removePart();
    m_map.setBlock(lastPos.x, lastPos.y, VOID);
    res = NOTHING;
  }

  return res;
}

