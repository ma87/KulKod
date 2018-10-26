#include "GameController.h"
#include <chrono>
#include <thread>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "launcher.h"
#include "rapl.h"

int main(int argc, char** argv)
{
  if (argc <= 1)
  {
    printf("usage: snake path_to_executable\n");
    return 1;
  }
  int number_snakes = argc - 1;

  int nrows = 32;
  int ncols = 32;  
  GameController control = GameController(nrows, ncols);

  int core = 0;
  FILE * fp = fopen("consumption.txt", "w");
  rapl_init(core);

  executable_t * childs = (executable_t *)malloc(number_snakes * sizeof(executable_t));
  for( int i = 0 ; i < number_snakes ; i++)
  {
    control.createSnake(i+1);
    childs[i].executable_path = argv[1 + i];
  }

  pid_t * pids = (pid_t *)malloc(number_snakes * sizeof(pid_t));
  char number_player[2];
  char nrows_str[3];
  char ncols_str[3];
  sprintf(nrows_str, "%d", nrows);
  sprintf(ncols_str, "%d", ncols);
  for( int i = 0 ; i < number_snakes ; i++)
  {
    sprintf(number_player, "%d", i+1);
    pids[i] = launch_process(&childs[i], 3, number_player, nrows_str, ncols_str);
  }

  int size_map = control.get_size_map();
  char * serialized_map = (char *)malloc(size_map * sizeof(char));
  memset(serialized_map, 0, size_map);

  double * energy_consumed = (double *)malloc(number_snakes * sizeof(double));
  for (int i = 0 ; i < number_snakes ; i++)
  {
    energy_consumed[i] = 0.0;
  }
  char direction;
  Direction * directions = (Direction *)malloc(number_snakes * sizeof(Direction));
  int crash_snake = 0;
  control.start();
  while(!crash_snake)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    system("clear");
    control.serialize_map(serialized_map, size_map);

    for (int i = 0 ; i < number_snakes ; i++)
    {
      rapl_before(fp, core);
      write_to_exe(&childs[i], serialized_map, size_map);
      read_from_exe(&childs[i], &direction, 1);  
      directions[i] = (Direction)direction;
      energy_consumed[i] += rapl_after(fp, core);
      std::cout << "Total energy consumed by player " << i + 1 << " = " << energy_consumed[i] << " J" << std::endl;
    }

    crash_snake = control.update(directions);
  }

  control.end(crash_snake);
  for (int i = 0 ; i < number_snakes ; i++)
  {
    kill_exe(pids[i]);
  }
  fclose(fp);
  fflush(stdout);
  return 0;
}
