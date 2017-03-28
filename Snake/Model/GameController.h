#ifndef __GAME_CONTROLLER____H
#define __GAME_CONTROLLER____H

#include "Map.h"
#include "Snake.h"
#include "Controller.h"

class GameController
{
  public:
    GameController(int nrows, int ncols);
    ~GameController();

    int update();
    int update(int nsamples);
    void loadSnake(char * dllName, short unsigned int player_number);
    void start();
    void end(int crash_player_number);

  private:
    Result updateMap(Snake * snake);

  private:
    AIManager m_aiManager;
    std::vector<Snake *> m_snakes;
    Map m_map;
};


#endif
