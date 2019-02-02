#include "energy_measurement.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUMBER_MEASURES 10

char * get_call_command(int core, int argc, char * argv[])
{

  size_t length_command = 0;

  // Add taskset -c 0 to restrict command on core
  length_command += strlen("taskset ");
  length_command += strlen("-c ");
  length_command += 2; // core id + ' '

  // Check command
  for (int i = 0 ; i < argc ; i++)
  {
    length_command += strlen(argv[i]) + 1;
  }

  // Redirect all output to /dev/null
  length_command += strlen("2>&1 > /dev/null") + 1;

  //printf("length command = %ld\n", length_command);
  char * call_command = (char *)malloc(length_command * sizeof(char));
  char char_core[3];
  sprintf(char_core,"%d ",core);
  strcpy(call_command, "taskset ");
  strcat(call_command, "-c ");
  strcat(call_command, char_core);

  for (int i = 0 ; i < argc ; i++)
  {
    strcat(call_command, argv[i]);
    strcat(call_command, " ");
  }

  strcat(call_command, "2>&1 > /dev/null");
  call_command[length_command] = '\0';

  return call_command;
}

int main(int argc, char * argv[])
{
  if (argc <= 1)
  {
    printf("usage: energy_measurement [-e energy_threshold -c 0:7] command [argument_1 argument_2 ...]\n");
    return 1;
  }

  energy_measurement_t energy_msrt;
  rapl_measurement_t rapl_msrt;

  double energy_measures[NUMBER_MEASURES];
  double time_measures[NUMBER_MEASURES];

  if (init_rapl_measurement(&rapl_msrt, 0))
  {
    printf("error: can't measure energy using RAPL\n");
    return 2;
  }

  init_energy_measurement(&energy_msrt, &rapl_msrt);

  long long counter = 0;
  double energy_threshold = 100.0;
  bool use_multiple_cores = true;
  int core = 0;
  int start_core,end_core,number_cores;
  int arguments_command_start = 1;
  if (!strcmp(argv[1], "-e") && !strcmp(argv[3], "-c"))
  {
    energy_threshold = atoi(argv[2]);
    arguments_command_start = 5;
    if (strlen(argv[4]) >= 3)
    {
      start_core = argv[4][0] - '0';
      end_core = argv[4][2] - '0';

      core = start_core;
    }
    else
    {
      core = atoi(argv[4]);
      use_multiple_cores = false;
    }
  }


  char * call_command = get_call_command(core, argc - arguments_command_start, argv+arguments_command_start);

  for (int i = 0 ; i < NUMBER_MEASURES ; i++)
  {
      sleep(1.0);
      counter = 0;
      energy_msrt.total_energy_consumed = 0.0;
      energy_msrt.total_time_elapsed    = 0.0;
      while (energy_msrt.total_energy_consumed < energy_threshold)
      {
        // Update CPU ID in call command. Look in get_call_command to compute position in char array
        call_command[11] = core + '0';

        if (init_rapl_measurement(&rapl_msrt, core))
        {
          printf("error: can't measure energy using RAPL\n");
          return 2;
        }

        start_energy_measurement(&energy_msrt);

        system(call_command);

        stop_energy_measurement(&energy_msrt);

        close_rapl_measurement(&rapl_msrt);

        counter++;
        if (use_multiple_cores)
        {
          core++;
          if (core > end_core)
          {
            core = start_core;
          }
        }
      }

      energy_measures[i] = energy_msrt.total_energy_consumed / counter;
      time_measures[i]   = energy_msrt.total_time_elapsed / counter;
  }

  double mean_energy = 0.0;
  double mean_time   = 0.0;
  for (int i = 0 ; i < NUMBER_MEASURES ; i++)
  {
      mean_energy += energy_measures[i];
      mean_time += time_measures[i];
  }

  mean_energy /= NUMBER_MEASURES;
  mean_time /= NUMBER_MEASURES;

  printf("%lf,%lf\n", mean_energy, mean_time);

  free(call_command);

  return 0;
}
