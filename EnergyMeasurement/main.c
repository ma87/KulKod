#include "rapl.h"

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

  

  return 0;
}
