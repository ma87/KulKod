#ifndef __LAUNCHER__H__
#define __LAUNCHER__H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
  int inpipefd[2];
  int outpipefd[2];
} pipes_t;

typedef struct
{
  pipes_t pipes;
  char * executable_path;
} executable_t;

pid_t launch_process(executable_t * exec, int argc, ...);
void init_pipes(pipes_t * pipes);
void close_unused_pipes(pipes_t * pipes);
int write_to_exe(executable_t * exec, char * bytes, int num_bytes);
int read_from_exe(executable_t * exec, char* bytes, int num_bytes);
int kill_exe(pid_t pid);
#ifdef __cplusplus
}
#endif


#endif
