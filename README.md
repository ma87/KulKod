# KulKod

Welcome in KulKod !

Objective is to code some AI and fight against others and see if your AI is the best.

## Snake

### Rules

Snake game classic: eat the apple and let the snake growing ! Here you have write the AI of your snake in an executable. Take a look in test folder to see how the executable interacts with the GameController to read/write data.
GameController is in charge of creating the map with apple and snake. An iteration contains few steps:
- GameController sends map to all executables with pipe.
- Each executable reads map in the standard input.
- Each executable writes direction of the snake to the standard output.
- GameController updates snake bodies one by one. If a snake eats apple, the snake is growing and a new apple is created. If a snake
hits wall or a part of a snake, game ends.

Here is how the data is encoded:

map is represented by number of rows x number of columns bytes.

Void byte:
0000|0000
Wall byte:
0000|0001
Apple byte:
0000|0010
Snake byte:
xxxx|0100
where player number is given in xxxx 

Direction of snake is encoded in one byte via a simple Enum:
UP=0, DOWN=1, LEFT=2, RIGHT=3

The executable is started by the GameController with 3 input arguments:
./path_to_executable player_number number_of_rows number_of_cols


In pseudo-code, executable looks like:

> **read** input arguments to retrieve player_number and how many bytes should be read
> **while** True:
>   current_map = read(stdin, number_bytes)
>   direction   = MYSNAKE.update_direction(current_map)
>   write(stdout, direction) 

Once game ends, GameController sends SIGTERM to all executables

### Execution

snake can be launched with 1 player:
./snake path_to_snake

This will create a file scores.txt indicating number of apples eaten by the snake.

snake can be launched with more than 1 player:
./snake path_to_snake_1 path_to_snake_2

This will create a file matches.txt indicating number of apples eaten by the snakes and which snake has crashed first.

### Energy measurement

GameController measures energy consumed by executable when it updates snake direction. The framework is made for Intel processors. To enable energy measurement:
> sudo modprobe msr
> sudo ./snake path_to_snakes

Ideas on how executable can be launched without admin rights or how energy measurement framework can read processors info without admin rights are welcome !

