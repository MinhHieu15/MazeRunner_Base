#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define MAZE_MAX_ROWS 64
#define MAZE_MAX_COLS 64
#define MAZE_MAX_NODES (MAZE_MAX_ROWS * MAZE_MAX_COLS)

/* Same convention as the Python version: row 0 is the bottom row, headings in degrees. */
typedef enum {
    HEAD_RIGHT = 0,
    HEAD_UP = 90,
    HEAD_LEFT = 180,
    HEAD_DOWN = 270
} Heading;

extern const Heading HEADINGS[4];

typedef struct {
    int row, col;
} Node;

typedef struct {
    int rows, cols;
    unsigned char cell[MAZE_MAX_ROWS][MAZE_MAX_COLS]; /* 1 = obstacle, 0 = free */
} Maze;

typedef bool VisitedGrid[MAZE_MAX_ROWS][MAZE_MAX_COLS];

void maze_init(Maze *m, int rows, int cols);
void maze_randomize(Maze *m);
bool maze_is_free(const Maze *m, Node n);

Node node_step(Node n, Heading h);
Heading heading_reverse(Heading h);

int maze_count_reachable(const Maze *m, Node start);

/* BFS from start to the closest free node that is not visited.
   Writes the headings to follow into path and returns their count, 0 if nothing is left. */
int maze_path_to_unvisited(const Maze *m, Node start, VisitedGrid visited, Heading *path);

#endif
