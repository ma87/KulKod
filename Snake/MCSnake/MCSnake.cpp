#include "MCSnake.h"

SnakeMC::SnakeMC(int player_number, int nrows, int ncols)
{
  m_player_number = player_number;
  m_headPosition = (Coords){0,0};
  m_direction = LEFT;

  m_nrows = nrows;
  m_ncols = ncols;
  m_current_index_path = 0;
}

SnakeMC::~SnakeMC()
{

};

void SnakeMC::logDebug(const char * format, ...)
{
  f = fopen("debug_mcsnake.txt", "a");
  va_list args;
  va_start(args, format);
  vfprintf(f, format, args);
  va_end(args); 
  fclose(f);
}

Direction SnakeMC::getDirection()
{
  return m_direction;
}

void SnakeMC::initHeadPosition(const char * map)
{
  int current_index = 0;
  for(unsigned int i=0 ; i<m_nrows ; i++)
  {
    for(unsigned int j=0 ; j<m_ncols ; j++)
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

Coords SnakeMC::getAppleCoords(const char * map)
{
  int current_index = 0;
  for(unsigned int i=0 ; i<m_nrows ; i++)
  {
    for (unsigned int j=0; j<m_ncols ; j++)
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

Block SnakeMC::getBlock(const char * map, int i, int j)
{
  // Matrix in column order
  // Example 2x2
  // [map(0) map(1)]
  // [map(2) map(3)]
  return (Block)map[j + m_ncols * i];
}

void SnakeMC::clearPath()
{
  logDebug("path is cleared\n");
  m_path.clear();
  m_current_index_path = 0;
}

bool SnakeMC::isBlockValid(const char * map, vector<ElemPath> & elems, Coords coord)
{
  if ((coord.x > 0) && (coord.x + 1 < m_nrows) && (coord.y > 0) && (coord.y + 1 < m_ncols))
  {
    Block block = getBlock(map, coord.x, coord.y);
    if ( (block == VOID) || (block == APPLE))
    {
      for (vector<ElemPath>::iterator elem = elems.begin() ; elem != elems.end() ; ++elem)
      {
        if ( (elem->coord.x == coord.x) && (elem->coord.y == coord.y) )
        {
          // Elem already in elems
          return false;
        }
      }
      // Block is empty and not already in elems
      return true;
    }
  }
  return false;
}

void SnakeMC::getValidNeighbours(const char * map, ElemPath * current_elem_path, vector<ElemPath> & elems)
{
  Coords neighbour;

  neighbour.x = current_elem_path->coord.x - 1;
  neighbour.y = current_elem_path->coord.y;
  if (isBlockValid(map, elems, neighbour))
  {
    elems.push_back( (ElemPath) {neighbour, current_elem_path->distance + 1, current_elem_path});
  }
  
  neighbour.x = current_elem_path->coord.x + 1;
  neighbour.y = current_elem_path->coord.y;
  if (isBlockValid(map, elems, neighbour))
  {
    elems.push_back( (ElemPath) {neighbour, current_elem_path->distance + 1, current_elem_path});
  }
  
  neighbour.x = current_elem_path->coord.x;
  neighbour.y = current_elem_path->coord.y - 1;
  if (isBlockValid(map, elems, neighbour))
  {
    elems.push_back( (ElemPath) {neighbour, current_elem_path->distance + 1, current_elem_path});
  }
  
  neighbour.x = current_elem_path->coord.x;
  neighbour.y = current_elem_path->coord.y + 1;
  if (isBlockValid(map, elems, neighbour))
  {
    elems.push_back( (ElemPath) {neighbour, current_elem_path->distance + 1, current_elem_path});
  }
}

void SnakeMC::testPath(const char * map)
{
  bool snake_in_path = false;

  for (int i = m_current_index_path ; i < m_path.size() ; i++)
  {
    if (getBlock(map, m_path[i].x, m_path[i].y) & SNAKE)
    {
      snake_in_path = true;
      break;
    }
  }

  if (snake_in_path)
    clearPath();
}
void SnakeMC::createPath(ElemPath & elem_apple)
{
  ElemPath * current_elem = &elem_apple;
  logDebug("distance = %d in cleanPath, head = %d %d\n", elem_apple.distance, m_headPosition.x, m_headPosition.y);
  m_path.clear();
  while (current_elem->distance > 0)
  {
    m_path.push_back((Coords) {current_elem->coord.x, current_elem->coord.y});
    current_elem = current_elem->parent;
  }
  std::reverse(m_path.begin(), m_path.end());

}

void SnakeMC::computePath(const char * map)
{
  vector<ElemPath> elems;
  elems.push_back((ElemPath) {m_headPosition, 0, NULL});
  int current_index = 0;
  ElemPath elem_apple;
  elem_apple.coord.x = 0;
  elem_apple.coord.y = 0;

  bool look_next = true;
  while (look_next)
  {
    if (current_index < elems.size())
    {
      getValidNeighbours(map, &elems[current_index], elems);
      for (vector<ElemPath>::iterator new_elem = elems.begin() ; new_elem != elems.end() ; ++new_elem)
      {
        if (getBlock(map, new_elem->coord.x, new_elem->coord.y) == APPLE)
        {
          logDebug("apple found at position %d %d\n", new_elem->coord.x, new_elem->coord.y);
          look_next = false;
          elem_apple = *new_elem;
        }
      }
    }
    else
    {
      logDebug("apple not found\n");
      look_next = false;
    }
    current_index ++;
  }

  if (elem_apple.coord.x != 0 && elem_apple.coord.y != 0)
  {
    createPath(elem_apple);
    logDebug("clean path ends, size path = %d\n", m_path.size());
  }
  else
  {
    logDebug("path is cleared from computePath\n");
    clearPath();
  }
}

void SnakeMC::updateDirection(const char * map)
{
  Coords apple = getAppleCoords(map);
  if ((m_headPosition.x == 0) && (m_headPosition.y == 0))
  {
    initHeadPosition(map);
    m_apple = apple;
  }

  if ( (m_apple.x != apple.x) || (m_apple.y != apple.y))
  {
    logDebug("new apple position in %d,%d\n", apple.x, apple.y);
    m_apple = apple;
    clearPath();
  } 

  testPath(map);

  if (m_path.size() == 0)
  {
    logDebug("compute path\n");
    computePath(map);
  }

  if (m_current_index_path == m_path.size())
  {
    logDebug("ERROR, recompute path\n");
    clearPath();
  }

  if (m_path.size() > 0)
  {
    Coords next_position = m_path[m_current_index_path];
    logDebug("step %d, position = (%d,%d)\n", m_current_index_path, next_position.x, next_position.y);
    if (next_position.x == m_headPosition.x)
    {
      if (next_position.y > m_headPosition.y)
      {
        m_direction = RIGHT;
      }
      else if (next_position.y < m_headPosition.y)
      {
        m_direction = LEFT;
      }
    }
    else
    {
      if (next_position.x > m_headPosition.x)
      {
        m_direction = DOWN;
      }
      else if (next_position.x < m_headPosition.x)
      {
        m_direction = UP;
      }
    }
    m_current_index_path++;
  }
  else
  {
    logDebug("path size is 0\n");
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
  logDebug("head is now at (%d,%d)\n", m_headPosition.x, m_headPosition.y);
}

std::string SnakeMC::getName()
{
  return "SnakeMC";
}
