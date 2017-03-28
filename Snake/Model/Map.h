#ifndef __SNAKE__MAP_H
#define __SNAKE__MAP_H

#include <vector>
#include <list>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

typedef enum
{
  NOTHING,
  APPLE_EATED,
  SNAKE_CRASH
} Result;

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
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

class Map
{
  public:
    Map(int nrows, int ncols);
    ~Map();
    void print();
    Coords getSize();
    bool createApple();
    Coords initSnakePosition(short unsigned int player_number);
    Block getBlock(unsigned int x, unsigned int y);
    void setBlock(unsigned int x, unsigned int y, short unsigned int player_number);
    void setBlock(unsigned int x, unsigned int y, Block block);
    Block ** getMap();

  private:
    Block ** m_map;
    unsigned int m_nrows;
    unsigned int m_ncols;
};

#endif
