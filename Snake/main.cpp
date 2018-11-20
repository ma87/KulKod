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

#include "energy_measurement.h"

int main(int argc, char** argv)
{
  if (argc <= 1)
  {
    printf("usage: snake [-v] path_to_executable\n");
    return 1;
  }
  int start_snake_index = 1;
  bool verbose_mode = false;
  if (!strcmp(argv[1], "-v"))
  {
    verbose_mode = true;
    start_snake_index = 2;
  }
  int number_snakes = argc - start_snake_index;

  int nrows = 32;
  int ncols = 32;
  GameController control = GameController(nrows, ncols);

  bool can_measure = false;
  int core = 0;
  rapl_measurement_t rapl_msrnt;
  int res = init_rapl_measurement(&rapl_msrnt, core);
  std::cout << " init rapl " << res << std::endl;
  can_measure = !res;
  FILE * fp;
  if (can_measure)
  {
    fp = fopen("consumption.txt", "w");
  }

  executable_t * childs = (executable_t *)malloc(number_snakes * sizeof(executable_t));
  for( int i = 0 ; i < number_snakes ; i++)
  {
    control.createSnake(i+1);
    childs[i].executable_path = argv[start_snake_index + i];
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

  energy_measurement_t * energy_msrnts = (energy_measurement_t *)malloc(number_snakes * sizeof(energy_measurement_t));
  for (int i = 0 ; i < number_snakes ; i++)
  {
    init_energy_measurement(&energy_msrnts[i], &rapl_msrnt);
  }
  char direction;
  Direction * directions = (Direction *)malloc(number_snakes * sizeof(Direction));
  int crash_snake = 0;
  control.start();
  while(!crash_snake)
  {
    if (verbose_mode)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      system("clear");
    }

    control.serialize_map(serialized_map, size_map);

    for (int i = 0 ; i < number_snakes ; i++)
    {
      if (can_measure)
      {
        start_energy_measurement(&energy_msrnts[i]);
      }
      write_to_exe(&childs[i], serialized_map, size_map);
      read_from_exe(&childs[i], &direction, 1);
      directions[i] = (Direction)direction;
      if (can_measure)
      {
        stop_energy_measurement(&energy_msrnts[i]);
        if (verbose_mode)
        {
          std::cout << "Total energy consumed by player " << i + 1 << " = " << energy_msrnts[i].total_energy_consumed << " J in " << energy_msrnts[i].total_time_elapsed << " us" << std::endl;
        }
      }
    }

    crash_snake = control.update(directions, verbose_mode);
  }

  control.end(crash_snake);
  for (int i = 0 ; i < number_snakes ; i++)
  {
    kill_exe(pids[i]);
  }
  if (can_measure)
  {
    for (int i = 0 ; i < number_snakes ; i++)
    {
      std::cout << "Total energy consumed by player " << i + 1 << " = " << energy_msrnts[i].total_energy_consumed << " J in " << energy_msrnts[i].total_time_elapsed << " us" << std::endl;
    }
  }
  close_rapl_measurement(&rapl_msrnt);
  return 0;
}
