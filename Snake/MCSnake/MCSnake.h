#ifndef __SNAKE__MC_H
#define __SNAKE__MC_H
#include <string>
#include <stdio.h>
#include <vector>
#include <stdarg.h>
#include <algorithm>

typedef enum
{
  VOID = 0x00,
  WALL = 0x01,
  APPLE = 0x02,
  SNAKE = 0x04
} Block;


typedef struct
{
  unsigned int x;
  unsigned int y;
} Coords;

typedef enum
{
  UP=0,
  DOWN,
  LEFT,
  RIGHT
} Direction;

using namespace std;

typedef struct ElemPath ElemPath;
struct ElemPath
{
  Coords coord;
  int    distance;
  ElemPath * parent;
}; 

class SnakeMC
{
  public:
   SnakeMC(int player_number, int nrows, int ncols);
   ~SnakeMC();

   std::string getName();
   void updateDirection(const char * block);
   Direction getDirection();

  private:
   void initHeadPosition(const char * map);
   Coords getAppleCoords(const char * map);
   Block getBlock(const char * map, int i, int j);
   void clearPath();
   void testPath(const char * map);
   bool isBlockValid(const char * map, vector<ElemPath> & elems, Coords coord);
   void getValidNeighbours(const char * map, ElemPath * current_elem_path, vector<ElemPath> & elems);
   void createPath(ElemPath & elem_apple);
   void computePath(const char * map);

  private:
   Coords m_headPosition;
   Coords m_apple;
   Direction m_direction;
   int m_player_number;
   int m_current_index_path;
   vector<Coords> m_path;
   int m_nrows;
   int m_ncols;

   // DEBUG
   FILE * f;
   void logDebug(const char * format, ...);
};

#endif
