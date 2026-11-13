#include "robot.h"
#include "rng.h"

void robot_init(Robot *r, Node start, Strategy strategy)
{
    r->pos = start;
    r->heading = HEAD_DOWN;
    r->came_from = HEAD_UP; /* Processors.lastDecision starts at 90 */
    r->strategy = strategy;
    r->path_len = r->path_idx = 0;
    for (int i = 0; i < MAZE_MAX_ROWS; i++)
        for (int j = 0; j < MAZE_MAX_COLS; j++)
            r->visited[i][j] = false;
    r->visited[start.row][start.col] = true;
    r->visited_count = 1;
    r->steps = 0;
}

int robot_sense(const Robot *r, const Maze *m, Heading out[4])
{
    int n = 0;
    for (int i = 0; i < 4; i++)
        if (maze_is_free(m, node_step(r->pos, HEADINGS[i])))
            out[n++] = HEADINGS[i];
    return n;
}

static Heading decide_random(const Robot *r, const Heading *options, int count)
{
    Heading choices[4];
    int n = 0;

    if (count == 1)
        return options[0];
    for (int i = 0; i < count; i++)
        if (options[i] != r->came_from)
            choices[n++] = options[i];
    return choices[rng_below(n)];
}

static Heading decide(Robot *r, const Maze *m, const Heading *options, int count)
{
    if (r->strategy == STRATEGY_EXPLORE) {
        Heading fresh[4];
        int n = 0;

        if (r->path_idx < r->path_len)
            return r->path[r->path_idx++];

        for (int i = 0; i < count; i++) {
            Node next = node_step(r->pos, options[i]);
            if (!r->visited[next.row][next.col])
                fresh[n++] = options[i];
        }
        if (n > 0)
            return fresh[rng_below(n)];

        r->path_len = maze_path_to_unvisited(m, r->pos, r->visited, r->path);
        r->path_idx = 0;
        if (r->path_len > 0)
            return r->path[r->path_idx++];
    }
    return decide_random(r, options, count);
}

bool robot_step(Robot *r, const Maze *m)
{
    Heading options[4];
    int count = robot_sense(r, m, options);
    if (count == 0)
        return false;

    r->heading = decide(r, m, options, count);
    r->came_from = heading_reverse(r->heading);
    r->pos = node_step(r->pos, r->heading);
    r->steps++;
    if (!r->visited[r->pos.row][r->pos.col]) {
        r->visited[r->pos.row][r->pos.col] = true;
        r->visited_count++;
    }
    return true;
}
