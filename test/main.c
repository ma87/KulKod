#include <stdio.h>

#include<unistd.h>
#include<sys/wait.h>
#include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>

#include "launcher.h"

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    printf("error test should be launched with path to 2 executables\n");
    return 1;
  }
  executable_t childs[2];
  
  childs[0].executable_path = argv[1];
  childs[1].executable_path = argv[2];

  pid_t pids[2];
  pids[0] = launch_process(&childs[0], 1, "1");
  pids[1] = launch_process(&childs[1], 1, "2");

  char buf[128];

  int counter = 3;
  int res = 0;
  while(counter--)
  {
    buf[0] = counter;
    write_to_exe(&childs[1], buf, 128);  
    read_from_exe(&childs[1], buf, 128);
    if (buf[0] != (counter + 1))
    {
      printf("error, launched process didnt return value + 1\n");
      res = 1;
    }
    write_to_exe(&childs[0], buf, 128);  
    read_from_exe(&childs[0], buf, 128);  
    if (buf[0] != (counter + 2))
    {
      printf("error, launched process didnt return value + 2\n");
      res = 2;
    }
  }

  kill_exe(pids[0]);
  kill_exe(pids[1]);

  return res;
}
