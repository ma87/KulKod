#!/usr/bin/env python3

from enum import IntEnum
import sys
import os

## Enum

class Block(IntEnum):
  VOID = 0
  WALL = 1
  APPLE = 2
  SNAKE = 4

class Direction(IntEnum):
  UP = 0
  DOWN = 1
  LEFT = 2
  RIGHT = 3


## INPUT ARGUMENTS

playerid = int(sys.argv[1])
nrows = int(sys.argv[2])
ncols = int(sys.argv[3])

number_blocks = nrows * ncols

# Parent process will kill exe when game is over
while True:
    # Read new map sent by parent process
    game_blocks = sys.stdin.buffer.read(number_blocks)

    # Write your implementation of snake
    direction = Direction.UP
    
    # Write direction to parent process
    nbytes = sys.stdout.buffer.write(direction.to_bytes(1, 'little'))
    sys.stdout.flush()
 
