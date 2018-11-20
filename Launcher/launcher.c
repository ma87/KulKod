#include "launcher.h"

pid_t launch_process(executable_t * exec, int argc, ...)
{
  va_list args;
  va_start(args, argc);

  init_pipes(&exec->pipes);

  pid_t pid = fork();
  if (pid == 0)
  {
    char ** argv = (char **)malloc((argc + 2)*sizeof(char *));
    argv[0] = (char *)basename(exec->executable_path);
    for (int i = 1 ; i <= argc ; i++)
    {
      argv[i] = va_arg(args, char *);
    }
    argv[argc+1] = (char *)NULL;

    dup2(exec->pipes.outpipefd[0], STDIN_FILENO);
    dup2(exec->pipes.inpipefd[1], STDOUT_FILENO);
    dup2(exec->pipes.inpipefd[1], STDERR_FILENO);

    prctl(PR_SET_PDEATHSIG, SIGTERM);

    execv(exec->executable_path, argv);
    // Nothing below this line should be executed by child process. If so,
    // it means that the execl function wasn't successfull, so lets exit:
    printf("ERROR launching child process\n");
    exit(1);
  }

  close_unused_pipes(&exec->pipes);

  va_end(args);
  return pid;
}

void init_pipes(pipes_t * pipes)
{
  pipe(pipes->inpipefd);
  pipe(pipes->outpipefd);
}

void close_unused_pipes(pipes_t * pipes)
{
  close(pipes->outpipefd[0]);
  close(pipes->inpipefd[1]);
}

int write_to_exe(executable_t * exec, char * bytes, int num_bytes)
{
  return write(exec->pipes.outpipefd[1], bytes, num_bytes);
}

int read_from_exe(executable_t * exec, char* bytes, int num_bytes)
{
  return read(exec->pipes.inpipefd[0], bytes, num_bytes);
}

int kill_exe(pid_t pid)
{
  int status;
  kill(pid, SIGTERM);
  waitpid(pid, &status, 0);
  return status;
}
