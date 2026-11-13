#include "maze.h"
#include "rng.h"

const Heading HEADINGS[4] = {HEAD_DOWN, HEAD_UP, HEAD_LEFT, HEAD_RIGHT};

void maze_init(Maze *m, int rows, int cols)
{
    m->rows = rows;
    m->cols = cols;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            m->cell[r][c] = (r == 0 || r == rows - 1 || c == 0 || c == cols - 1);
}

/* 7 in 22 chance of an obstacle, the same odds as the choice() tuple in core/map.py */
void maze_randomize(Maze *m)
{
    for (int r = 1; r < m->rows - 1; r++)
        for (int c = 1; c < m->cols - 1; c++)
            m->cell[r][c] = rng_below(22) < 7;
}

bool maze_is_free(const Maze *m, Node n)
{
    return n.row >= 0 && n.row < m->rows && n.col >= 0 && n.col < m->cols
        && m->cell[n.row][n.col] == 0;
}

Node node_step(Node n, Heading h)
{
    switch (h) {
    case HEAD_UP:    n.row++; break;
    case HEAD_DOWN:  n.row--; break;
    case HEAD_RIGHT: n.col++; break;
    case HEAD_LEFT:  n.col--; break;
    }
    return n;
}

Heading heading_reverse(Heading h)
{
    return (Heading)((h + 180) % 360);
}

int maze_count_reachable(const Maze *m, Node start)
{
    static VisitedGrid seen;
    static Node queue[MAZE_MAX_NODES];
    int head = 0, tail = 0;

    for (int r = 0; r < m->rows; r++)
        for (int c = 0; c < m->cols; c++)
            seen[r][c] = false;

    seen[start.row][start.col] = true;
    queue[tail++] = start;
    while (head < tail) {
        Node n = queue[head++];
        for (int i = 0; i < 4; i++) {
            Node next = node_step(n, HEADINGS[i]);
            if (maze_is_free(m, next) && !seen[next.row][next.col]) {
                seen[next.row][next.col] = true;
                queue[tail++] = next;
            }
        }
    }
    return tail;
}

int maze_path_to_unvisited(const Maze *m, Node start, VisitedGrid visited, Heading *path)
{
    static VisitedGrid seen;
    static Node parent[MAZE_MAX_ROWS][MAZE_MAX_COLS];
    static Heading via[MAZE_MAX_ROWS][MAZE_MAX_COLS];
    static Node queue[MAZE_MAX_NODES];
    int head = 0, tail = 0;

    for (int r = 0; r < m->rows; r++)
        for (int c = 0; c < m->cols; c++)
            seen[r][c] = false;

    seen[start.row][start.col] = true;
    queue[tail++] = start;
    while (head < tail) {
        Node n = queue[head++];
        if (!visited[n.row][n.col]) {
            int len = 0;
            while (n.row != start.row || n.col != start.col) {
                path[len++] = via[n.row][n.col];
                n = parent[n.row][n.col];
            }
            for (int i = 0; i < len / 2; i++) {
                Heading t = path[i];
                path[i] = path[len - 1 - i];
                path[len - 1 - i] = t;
            }
            return len;
        }
        for (int i = 0; i < 4; i++) {
            Node next = node_step(n, HEADINGS[i]);
            if (maze_is_free(m, next) && !seen[next.row][next.col]) {
                seen[next.row][next.col] = true;
                parent[next.row][next.col] = n;
                via[next.row][next.col] = HEADINGS[i];
                queue[tail++] = next;
            }
        }
    }
    return 0;
}
