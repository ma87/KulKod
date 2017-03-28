#include "Map.h"


Map::Map(int nrows, int ncols)
{
  m_map = (Block **)malloc(nrows * sizeof(Block *));
  for(int i=0 ; i<nrows; i++)
  {
    m_map[i] = (Block *)malloc(ncols * sizeof(Block));
    memset(m_map[i], 0, ncols * sizeof(Block));
  }

  // initialize walls
  for(int j=0 ; j<ncols ; j++)
  {
    m_map[0][j] = WALL;
    m_map[nrows - 1][j] = WALL;
  }

  for(int i=0 ; i<nrows ; i++)
  {
    m_map[i][0] = WALL;
    m_map[i][ncols - 1] = WALL;
  }

  m_nrows = nrows;
  m_ncols = ncols;
}

Map::~Map()
{
  for(int i=0 ; i<m_nrows; i++)
  {
    free(m_map[i]);
  }
  free(m_map);
}

Coords Map::getSize()
{
  return (Coords) {m_nrows, m_ncols};
}

Block ** Map::getMap()
{
  return m_map;
}

bool Map::createApple()
{
  bool correctPosition = false;
  unsigned int x;
  unsigned int y;
  while(!correctPosition)
  {
    x = rand() % m_nrows;
    y = rand() % m_ncols;
    correctPosition = m_map[x][y] == VOID;
  }

  m_map[x][y] = APPLE;
  return true;
}

Coords Map::initSnakePosition(short unsigned int player_number)
{
  bool correctPosition = false;
  unsigned int x;
  unsigned int y;
  while(!correctPosition)
  {
    x = rand() % m_nrows;
    y = rand() % m_ncols;
    correctPosition = m_map[x][y] == VOID;
  }

  setBlock(x, y, player_number);

  return (Coords){x, y};
}

void Map::print()
{
  std::vector<std::string> output;
  for(int i=0 ; i<m_nrows ; i++)
  {
    for(int j=0 ; j<m_ncols ; j++)
    {
      short int block_type = m_map[i][j] & 0x0F;
      short int number_player;
      switch(block_type)
      {
        case WALL:
          std::cout << "X";
        break;
        case VOID:
          std::cout << " ";
        break;
        case SNAKE:
          number_player = m_map[i][j] >> 16;
          std::cout << number_player;
        break;
        case APPLE:
          std::cout << "0";
        break;
      }
    }
    std::cout << "\n";
  }
}

Block Map::getBlock(unsigned int x, unsigned int y)
{
  return m_map[x][y];
}

void Map::setBlock(unsigned int x, unsigned int y, short unsigned int player_number)
{
  int block_snake = player_number << 16 | SNAKE;
  m_map[x][y] = (Block)block_snake;

  short int block_type = block_snake & 0x0F;

  short int number_player = block_snake >> 16;
}

void Map::setBlock(unsigned int x, unsigned int y, Block block)
{
  m_map[x][y] = block;
}
