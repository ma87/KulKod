#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

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

  // Infinite loop until parent sends SIGTERM
  char direction;
  while (1)
  {
    // Read size_map bytes in rbuf
    size_t r = fread(rbuf, 1, size_map, stdin);

    // Implement your solution here depending on rbuf: set direction to LEFT, RIGHT, UP, DOWN
    direction = RIGHT;

    // Write to parent process the direction of the snake
    size_t written = fwrite(&direction, 1, 1, stdout);
    fflush(stdout);
  }
}
