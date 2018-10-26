#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

FILE *f;

// Function called when game is finished
void onExit(int i)
{
  fclose(f);
  exit(0);
}

int main(int argc, char** argv)
{
  signal(SIGTERM, onExit);
  char rbuf[128];
  memset(rbuf, 0, 128);
  char filename[50];

  strcpy(filename, argv[1]);
  strcat(filename, "_test.txt");
  
  f = fopen(filename, "w");

  // Infinite loop until parent sends SIGTERM
  int counter = 1;
  while (1)
  {
    fprintf(f, "wait to read\n");
    size_t r = fread(rbuf, 1, 128, stdin);
    // Implement your solution here
    char value = rbuf[0];
    fprintf(f, "read value %c %ld write\n", value, r);
 
    rbuf[0]++;
    value = rbuf[0];
    //rbuf[255] = '\n';
    size_t written = fwrite(rbuf, 1, 128, stdout);
    fflush(stdout);
  }
}
