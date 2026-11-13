CC = gcc
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -pedantic

SRC = main.c maze.c robot.c rng.c
HDR = maze.h robot.h rng.h

mazerunner: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f mazerunner mazerunner.exe

.PHONY: clean
