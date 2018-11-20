#ifndef __GAME_CONTROLLER____H
#define __GAME_CONTROLLER____H

#include "Map.h"
#include "Snake.h"

class GameController
{
  public:
    GameController(int nrows, int ncols);
    ~GameController();

    int update(Direction * snake_direction, bool verbose_mode);
    void createSnake(int player_number);
    void start();
    void end(int crash_player_number);
    int get_size_map();
    int serialize_map(char * serialized_map, int size_buffer);
  private:
    Result updateMap(Snake & snake);

  private:
    std::vector<Snake> m_snakes;
    Map m_map;
};


#endif
