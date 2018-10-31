#!/usr/bin/env python3

from enum import IntEnum
import sys
import os

## INPUT ARGUMENTS

playerid = int(sys.argv[1])
nrows = int(sys.argv[2])
ncols = int(sys.argv[3])

number_blocks = nrows * ncols


## TOOLS

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

class Coords:
    def __init__(self, x, y):
        self.row = x
        self.col = y


def get_next_direction(direction):
    if direction==Direction.LEFT:
        return Direction.UP
    elif direction==Direction.UP:
        return Direction.RIGHT
    elif direction==Direction.RIGHT:
        return Direction.DOWN
    elif direction==Direction.DOWN:
        return Direction.LEFT
    return direction

## SNAKE

class Snake:
    def __init__(self, player_id, nrows, ncols):
        self.direction = Direction.LEFT
        self.head = None
        self.nrows = nrows
        self.ncols = ncols
        self.player_id = player_id

    def get_block(self, map, coord):
        return map[coord.col + coord.row*ncols]

    def get_apple(self, map):
        for i in range(nrows):
            for j in range(ncols):
                if self.get_block(map, Coords(i, j)) == Block.APPLE:
                    return Coords(i, j)
        return None

  
    def set_head_position(self, head):
        self.head = head

    def get_head_position(self, map):
        for i in range(self.nrows - 1):
            for j in range(self.ncols - 1):
                block = self.get_block(map, Coords(i,j))
                if block & Block.SNAKE:
                   block_playerid = block >> 4
                   if block_playerid==self.player_id:
                       self.set_head_position(Coords(i, j))
                       return

    def forward(self, map, start_position, objective, direction):
        traversed_nodes = []
        current_position = start_position
        while current_position.row != objective.row or current_position.col != objective.col:
            if direction==Direction.LEFT:
                current_position.col -= 1
            elif direction==Direction.UP:
                current_position.row -= 1
            elif direction==Direction.RIGHT:
                current_position.col += 1
            elif direction==Direction.DOWN:
                current_position.row += 1
            if self.get_block(map, current_position) != VOID:
                return traversed_nodes
            else:
                traversed_nodes.append(current_position)
        return traversed_nodes


    def find_path(self, map, apple, position, direction):
        traversed_nodes = self.forward(map, position, apple, direction)
        if len(traversed_nodes) == 0:
            return -1


        return len(traversed_points)

    def get_valid_neighbours(self, map, current_node):
        coords = [Coords(current_node.row - 1, current_node.col), 
                  Coords(current_node.row + 1, current_node.col),
                  Coords(current_node.row, current_node.col - 1),
                  Coords(current_node.row, current_node.col + 1),
                 ]
        return [coord for coord in coords if self.get_block(map, coord.row, coord.col)

    def update_direction(self, map):
        if self.head is None:
            self.get_head_position(map)
        apple = self.get_apple(map)
        if apple is None:
            return
 
        nodes = [(apple, 0)]
        look_next = True
        current_index = 1
        while look_next:
            
        direction = self.direction
        minimum_score = nrows * ncols
        for i in range(2):
            score = self.find_path(map, apple, self.head, direction)
            direction = get_next_direction(direction)
            if score < minimum_score:
                self.direction = direction
                minimum_score = score
        


## I/O with main process
snake = Snake(playerid, nrows, ncols)
while True:
    map = sys.stdin.buffer.read(number_blocks)

    snake.update_direction(map)

    nbytes = sys.stdout.buffer.write(snake.direction.to_bytes(1, 'little'))
    sys.stdout.flush()
