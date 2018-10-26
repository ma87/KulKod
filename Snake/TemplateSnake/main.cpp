#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

FILE *f;

typedef enum
{
  UP=0,
  DOWN,
  LEFT,
  RIGHT
} Direction;

// Function called when game is finished
void onExit(int i)
{
  fclose(f);
  exit(0);
}

int main(int argc, char** argv)
{
  signal(SIGTERM, onExit);
  if (argc != 4)
  {
    return 1;
  }

  int player_number = atoi(argv[1]);
  int nrows = atoi(argv[2]);
  int ncols = atoi(argv[3]);

  int size_map = nrows * ncols;
  char * rbuf = (char *)malloc(size_map);

  memset(rbuf, 0, size_map);
  char filename[50];

  strcpy(filename, argv[1]);
  strcat(filename, ".txt");
  
  f = fopen(filename, "w");
  fprintf(f, "filename %d %d %d = %s\n", player_number, nrows, ncols, filename);

  // Infinite loop until parent sends SIGTERM
  char direction;
  while (1)
  {
    fprintf(f, "wait to read map\n");
    size_t r = fread(rbuf, 1, size_map, stdin);

    // Implement your solution here: set direction to LEFT, RIGHT, UP, DOWN
    char value = rbuf[0];
    char value2 = rbuf[35];
    fprintf(f, "read value %c %ld write\n", value, r);
    fprintf(f, "read value2 %c %ld write\n", value2, r);
 
    direction = RIGHT;

    size_t written = fwrite(&direction, 1, 1, stdout);
    fflush(stdout);
  }
}
