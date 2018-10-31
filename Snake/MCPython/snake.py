#!/usr/bin/env python3

from enum import IntEnum
import sys
import os

## INPUT ARGUMENTS

playerid = int(sys.argv[1])
nrows = int(sys.argv[2])
ncols = int(sys.argv[3])

number_blocks = nrows * ncols


def log_debug(msg):
    pass
    #with open("py_coucou.txt", "a") as f:
    #    f.write(msg + "\n")

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

class Elem:
    def __init__(self, coord, distance, parent):
        self.coord = coord
        self.distance = distance
        self.parent = parent


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
        self.path = []
        self.apple = None
        self.current_index_path = 0

    def get_block(self, game_blocks, coord):
        #log_debug("coord = " + str(coord.row) + " " + str(coord.col))
        return game_blocks[coord.col + coord.row*self.ncols]

    def get_apple(self, game_blocks):
        for i in range(nrows):
            for j in range(ncols):
                if self.get_block(game_blocks, Coords(i, j)) == Block.APPLE:
                    return Coords(i, j)
        return None

    def get_head_position(self, game_blocks):
        for i in range(self.nrows - 1):
            for j in range(self.ncols - 1):
                block = self.get_block(game_blocks, Coords(i,j))
                if block & Block.SNAKE:
                   block_playerid = block >> 4
                   if block_playerid==self.player_id:
                       self.head = Coords(i, j)
                       return

    def is_block_valid(self, game_blocks, coord):
        block = self.get_block(game_blocks, coord)
        return block == Block.VOID or block == Block.APPLE

    def get_valid_neighbours(self, game_blocks, current_elem):
        coords = []
        if current_elem.coord.row > 0:
            coords.append(Coords(current_elem.coord.row - 1, current_elem.coord.col))
        if current_elem.coord.row + 1 < self.nrows:
            coords.append(Coords(current_elem.coord.row + 1, current_elem.coord.col))
        if current_elem.coord.col > 0:
            coords.append(Coords(current_elem.coord.row, current_elem.coord.col - 1))
        if current_elem.coord.col + 1 < self.ncols:
            coords.append(Coords(current_elem.coord.row, current_elem.coord.col + 1))
        return [Elem(coord, current_elem.distance + 1, current_elem) for coord in coords if self.is_block_valid(game_blocks, coord)]


    def test_path(self, game_blocks):
        snake_in_path = False
        for elem in self.path[self.current_index_path:]:
            if self.get_block(game_blocks, elem.coord) & Block.SNAKE:
                snake_in_path = True
                break

        if snake_in_path:
            self.clear_path()

    def add_path(self, new_elems):
        for new_elem in new_elems:
            add_new_elem = True
            for elem in self.path:
                if elem.coord.row == new_elem.coord.row and elem.coord.col == new_elem.coord.col:
                    if new_elem.distance < elem.distance:
                        self.path.remove(elem)         
                        self.path.append(new_elem)
                    else:
                        add_new_elem = False
                    break
            if add_new_elem:
                self.path.append(new_elem)

    def clean_path(self, elem_apple):
        cleaned_path = []
        current_elem = elem_apple
        while current_elem.coord != self.head:
            cleaned_path.append(Elem(current_elem.coord, current_elem.distance, current_elem.parent))
            current_elem = current_elem.parent
        self.path.clear()
        cleaned_path.reverse()
        self.path = cleaned_path


    def compute_path(self, game_blocks):
        self.path = [Elem(self.head, 0, None)]
        look_next = True
        current_index = 0
        elem_apple = None
    
        while look_next:
            if current_index < len(self.path):
                new_elems = self.get_valid_neighbours(game_blocks, self.path[current_index])
                for elem in new_elems:
                    if self.get_block(game_blocks, elem.coord) == Block.APPLE:
                        look_next = False
                        elem_apple = elem
                self.add_path(new_elems)
            else:
                # No new elems can be added: Snake is blocked ?
                look_next = False
            current_index += 1
        if elem_apple is not None:
            self.clean_path(elem_apple)
        else:
            log_debug("apple not found clear path")
            self.clear_path()

    def update_head_position(self):
        if self.direction==Direction.LEFT:
            self.head.col -= 1
        elif self.direction==Direction.UP:
            self.head.row -= 1
        elif self.direction==Direction.RIGHT:
            self.head.col += 1
        elif self.direction==Direction.DOWN:
            self.head.row += 1

    def clear_path(self):
        log_debug("path cleared")
        self.path.clear()
        self.current_index_path = 0

    def update_direction(self, game_blocks):
        if self.head is None:
            self.get_head_position(game_blocks)
        apple = self.get_apple(game_blocks)
        if self.apple is None:
            self.apple = apple
        elif self.apple.row != apple.row or self.apple.col != apple.col:
            log_debug("apple has been eaten, recompute path")
            self.apple = apple
            self.clear_path()

        # Check if another snake block path
        self.test_path(game_blocks)

        if len(self.path) == 0:
            self.compute_path(game_blocks)

                            
        if len(self.path) > 0:
            log_debug("current_index_path / total " + str(self.current_index_path) + " / " + str(len(self.path)))
            next_position = self.path[self.current_index_path].coord
            if next_position.row == self.head.row:
                if next_position.col > self.head.col:
                    self.direction = Direction.RIGHT
                else:
                    self.direction = Direction.LEFT
            else:
                if next_position.row > self.head.row:
                    self.direction = Direction.DOWN
                else:
                    self.direction = Direction.UP
            self.current_index_path += 1

        self.update_head_position()


## I/O with main process
snake = Snake(playerid, nrows, ncols)
while True:
    game_blocks = sys.stdin.buffer.read(number_blocks)
    if len(game_blocks)==number_blocks:
        snake.update_direction(game_blocks)
    else:
        break
    nbytes = sys.stdout.buffer.write(snake.direction.to_bytes(1, 'little'))
    sys.stdout.flush()
    
