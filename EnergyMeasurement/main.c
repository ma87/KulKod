#include "energy_measurement.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <getopt.h>

#define NUMBER_MEASURES 1

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

  printf("call command = %s\n", call_command);
  return call_command;
}

bool is_below_energy_threshold(energy_measurement_t * energy_msrt, int number_plugins, double energy_threshold)
{
  for (int i = 0 ; i < number_plugins ; i++)
  {
    if (energy_msrt[i].total_energy_consumed < energy_threshold)
    {
      return true;
    }
  }
  return false;
}

int main(int argc, char * argv[])
{
  bool use_rapl = false;
  bool use_tplink = false;
  double energy_threshold = 100.0;
  char * target_name;
  char * cores;
  int c;
    while (1)
    {
      static struct option long_options[] =
        {
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"rapl",    no_argument,       0, 'r'},
          {"tplink",  no_argument,       0, 'p'},
          {"target",  required_argument, 0, 't'},
          {"cores",   required_argument, 0, 'c'},
          {"energy",  optional_argument, 0, 'e'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "rpt:e:c:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        
        case 'r':
          printf("rapl activated\n");
	  use_rapl = 1;
          break;

        case 'p':
          printf("tplink activated\n");
	  use_tplink = 1;
          break;

        case 't':
	  target_name = optarg;
          printf ("tplink host = %s\n", optarg);
          break;

        case 'e':
          printf ("energy with value `%s'\n", optarg);
	  energy_threshold = (double)atoi(optarg);
          break;

	case 'c':
	  cores = optarg;
	  printf("cores = %s\n", cores);
	  break;

        default:
          abort ();
        }
    }

    /* Print any remaining command line arguments (not options). */
    /*if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }*/
  
  energy_measurement_t energy_msrt[NUMBER_PLUGINS];
  rapl_measurement_t rapl_msrt;
  energy_plugin_t rapl_plugin;
  tplink_measurement_t tplink_msrt;
  energy_plugin_t tplink_plugin;

  double energy_measures_rapl[NUMBER_MEASURES];
  double energy_measures_tplink[NUMBER_MEASURES];
  double time_measures[NUMBER_MEASURES];

  int current_plugin = 0;
  if (use_rapl)
  {
    if (init_rapl_measurement(&rapl_msrt, 0))
    {
      printf("error: can't measure energy using RAPL\n");
      return 2;
    }

    rapl_plugin.data = &rapl_msrt;
    rapl_plugin.f    = get_current_energy_rapl;
    rapl_plugin.p    = RAPL;

    init_energy_measurement(&energy_msrt[current_plugin], &rapl_plugin);
    current_plugin++;
  }

  if (use_tplink)
  {
    if (init_tplink_plugin(&tplink_msrt, target_name, 0.001))
    {
      printf("error: can't measure energy using TPLINK\n");
      return 2;
    }
    tplink_plugin.data = &tplink_msrt;
    tplink_plugin.f    = get_current_energy_tplink;
    tplink_plugin.p    = TPLINK_HS110;
    init_energy_measurement(&energy_msrt[current_plugin], &tplink_plugin);
    current_plugin++;
  }

  if (current_plugin == 0)
  {
    printf("error: at least one plugin needs to be used\n");
    return 3;
  }
  
  long long counter = 0;
  int core = 0;
  int start_core,end_core,number_cores;
  bool use_multiple_cores = true;
  printf("core = %d\n", core);
  if (strlen(cores) >= 3)
    {
      start_core = cores[0] - '0';
      end_core = cores[2] - '0';

      core = start_core;
    }
    else
    {
      core = atoi(cores);
      use_multiple_cores = false;
    }

printf("core = %d\n", core);
    
  char * call_command = get_call_command(core, argc - optind, argv+optind);
    printf("call command\n");

  for (int i = 0 ; i < NUMBER_MEASURES ; i++)
  {
      sleep(1.0);
      counter = 0;
      for (int p = 0 ; p < current_plugin ; p++)
      {
   	energy_msrt[p].total_energy_consumed = 0.0;
        energy_msrt[p].total_time_elapsed    = 0.0;
      }
      while (is_below_energy_threshold(energy_msrt, current_plugin, energy_threshold))
      {
        // Update CPU ID in call command. Look in get_call_command to compute position in char array
        call_command[11] = core + '0';

        for (int p = 0 ; p < current_plugin ; p++)
          start_energy_measurement(&energy_msrt[p]);

        system(call_command);

        for (int p = 0 ; p < current_plugin ; p++)
          stop_energy_measurement(&energy_msrt[p]);

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

        
      for (int p = 0 ; p < current_plugin ; p++)
      {
	if (energy_msrt[p].plugin->p == RAPL)
		energy_measures_rapl[i] = energy_msrt[p].total_energy_consumed / counter;
	else if (energy_msrt[p].plugin->p == TPLINK_HS110)
		energy_measures_tplink[i] = energy_msrt[p].total_energy_consumed / counter;
      }
      time_measures[i]   = energy_msrt[0].total_time_elapsed / counter;
  }

  double mean_energy_rapl = 0.0;
  double mean_energy_tplink = 0.0;
  double mean_time   = 0.0;
  for (int i = 0 ; i < NUMBER_MEASURES ; i++)
  {
      mean_energy_rapl += energy_measures_rapl[i];
      mean_energy_tplink += energy_measures_tplink[i];
      mean_time += time_measures[i];
  }

  mean_energy_rapl /= NUMBER_MEASURES;
  mean_energy_tplink /= NUMBER_MEASURES;
  mean_time /= NUMBER_MEASURES;

  printf("%lf,%lf,%lf,%lf,%lld\n", mean_energy_rapl, mean_energy_tplink, mean_time, energy_threshold, counter);

  free(call_command);
  return 0;
}
