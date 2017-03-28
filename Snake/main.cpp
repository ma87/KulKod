// #include "Snake.h"
// #include "Map.h"
#include "GameController.h"
#include <chrono>
#include <thread>

using std::cout;
using std::cerr;

int main(int argc, char * argv[])
{
  if (argc > 1)
  {
    GameController control = GameController(30, 30);
    for (int i = 1 ; i < argc ; i++)
    {
      control.loadSnake(argv[i], i);
    }

    control.start();
    int crash_snake = 0;
    while (crash_snake == 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      system("clear");
      crash_snake = control.update();
    }

    control.end(crash_snake);
  }
  else
  {
    std::cout << "usage: snake path_to_snake_implementation" << std::endl;
    return 1;
  }
  return 0;
}
