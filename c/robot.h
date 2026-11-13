#ifndef ROBOT_H
#define ROBOT_H

#include "maze.h"

typedef enum {
    STRATEGY_EXPLORE, /* unvisited neighbours first, BFS to the nearest unvisited node when stuck */
    STRATEGY_RANDOM   /* random walk that never turns straight back unless it has to */
} Strategy;

typedef struct {
    Node pos;
    Heading heading;
    Heading came_from;
    Strategy strategy;

    Heading path[MAZE_MAX_NODES];
    int path_len, path_idx;

    VisitedGrid visited;
    int visited_count;
    int steps;
} Robot;

void robot_init(Robot *r, Node start, Strategy strategy);

/* Free headings around the robot, like Sensors.getOutput() in Python. */
int robot_sense(const Robot *r, const Maze *m, Heading out[4]);

/* Move one node. Returns false if the robot is boxed in. */
bool robot_step(Robot *r, const Maze *m);

#endif
