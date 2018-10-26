#ifndef __SNAKE__MC_H
#define __SNAKE__MC_H
#include <string>
#include <stdio.h>
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

class SnakeMC
{
  public:
   SnakeMC(int player_number);
   ~SnakeMC();

   std::string getName();
   void updateDirection(const char * block, int nrows, int ncols);
   Direction getDirection();

  private:
   void initHeadPosition(const char * map, int nrows, int ncols);
   Coords getAppleCoords(const char * map, int nrows, int ncols);
   Block getBlock(const char * map, int i, int j, int ncols);

  private:
   Coords m_headPosition;
   Direction m_direction;
   int m_player_number;

   // DEBUG
   FILE * f;
};

#endif
