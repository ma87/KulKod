#include "energy_measurement.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
  if (argc <= 1)
  {
    printf("usage: energy_measurement command [argument_1 argument_2 ...]\n");
    return 1;
  }

  energy_measurement_t energy_msrt;
  rapl_measurement_t rapl_msrt;

  if (init_rapl_measurement(&rapl_msrt, 0))
  {
    printf("error: can't measure energy using RAPL\n");
    return 2;
  }

  init_energy_measurement(&energy_msrt, &rapl_msrt);

  start_energy_measurement(&energy_msrt);

  long long counter = 0;
  double energy_threshold = 100.0;
  while (energy_msrt.total_energy_consumed < energy_threshold)
  {
    int pid = fork();
    if (pid == 0)
    {
      // child starts command passed in argument
  		int exec_status = execvp(argv[1], argv+1);
    }
    else
    {

      int status = 1;
  		waitpid(pid, &status, WNOHANG);
      while (status)
      {
        //usleep(25000);
        trigger_energy_measurement(&energy_msrt);
        //printf("\r%lf J consumed in %lf us", energy_msrt.total_energy_consumed, energy_msrt.total_time_elapsed);
        fflush(stdout);
        waitpid(pid, &status, WNOHANG);
      }
    }

    counter++;
  }

  printf("\nIt took %lld iterations to consume %lf J for program %s\nIt consumed on average %lf J in %lf us per iteration\n", counter, energy_threshold, argv[1], energy_msrt.total_energy_consumed / counter, energy_msrt.total_time_elapsed / counter);


  return 0;
}
