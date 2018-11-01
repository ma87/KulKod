#include "Map.h"


Map::Map(int nrows, int ncols)
{
  m_map = (Block *)malloc(nrows * ncols * sizeof(Block));
  memset(m_map, 0, nrows * ncols * sizeof(Block));

  m_nrows = nrows;
  m_ncols = ncols;

  // initialize walls
  for(int j=0 ; j<ncols ; j++)
  {
    setBlock(0, j, WALL);
    setBlock(nrows - 1, j, WALL);
  }

  for (int j = 8 ; j < m_ncols - 8 ; j++)
  {
    setBlock(nrows / 2, j, WALL);
  }

  for(int i=0 ; i<nrows ; i++)
  {
    setBlock(i, 0, WALL);
    setBlock(i, ncols - 1, WALL);
  }

}

Map::~Map()
{
  free(m_map);
}

Coords Map::getSize()
{
  return (Coords) {m_nrows, m_ncols};
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
    correctPosition = getBlock(x, y) == VOID;
  }

  setBlock(x, y, APPLE);
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
    correctPosition = getBlock(x, y) == VOID;
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
      short int block_type = getBlock(i, j) & 0x0F;
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
          number_player = getBlock(i, j) >> 16;
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

char Map::convertBlockToByte(int row, int col)
{
  Block block = getBlock(row,col);
  char c;
  if (block & SNAKE)
  {
    short unsigned int number_player = block >> 16;
    c = (number_player << 4) | SNAKE;
  }
  else
  {
    c = (char)block;
  }
  return c;
}

int Map::serialize(char * serialize_map, int size_buffer)
{
  if (size_buffer != (m_nrows * m_ncols))
  {
    std::cout << "error during serialize: buffer size is invalid" << std::endl;
    return 1;
  }

  for (int i=0 ; i < m_nrows ; i++)
  {
    for (int j = 0 ; j < m_ncols ; j++)
    {
     serialize_map[j + i * m_ncols] = convertBlockToByte(i, j);
    }
  }
   
  return 0;
}


Block Map::getBlock(unsigned int x, unsigned int y)
{
  return m_map[y + x * m_ncols];
}

void Map::setBlock(unsigned int x, unsigned int y, short unsigned int player_number)
{
  int block_snake = player_number << 16 | SNAKE;
  m_map[y + x * m_ncols] = (Block)block_snake;
}

void Map::setBlock(unsigned int x, unsigned int y, Block block)
{
  m_map[y + x * m_ncols] = block;
}
