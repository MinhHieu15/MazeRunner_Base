#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "maze.h"
#include "rng.h"
#include "robot.h"

typedef struct {
    unsigned seed;
    int rows, cols;
    int max_steps;
    int delay_ms;
    Strategy strategy;
    bool quiet;
} Options;

static void usage(const char *prog)
{
    printf("Usage: %s [options]\n"
           "  --seed N          random seed (default: current time)\n"
           "  --rows N          map rows incl. border, 5..%d (default 30)\n"
           "  --cols N          map columns incl. border, 7..%d (default 30)\n"
           "  --steps N         stop after N moves (default 2000)\n"
           "  --delay MS        pause between frames in ms (default 80)\n"
           "  --strategy NAME   explore | random (default explore)\n"
           "  --quiet           no animation, print the final map and stats only\n",
           prog, MAZE_MAX_ROWS, MAZE_MAX_COLS);
}

static int parse_int(const char *flag, const char *value, int lo, int hi)
{
    char *end;
    long v = value ? strtol(value, &end, 10) : 0;
    if (!value || *end != '\0' || v < lo || v > hi) {
        fprintf(stderr, "%s expects a number in %d..%d, got '%s'\n", flag, lo, hi, value ? value : "");
        exit(2);
    }
    return (int)v;
}

static Options parse_args(int argc, char **argv)
{
    Options o = {(unsigned)time(NULL), 30, 30, 2000, 80, STRATEGY_EXPLORE, false};

    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        const char *v = i + 1 < argc ? argv[i + 1] : NULL;
        if (!strcmp(a, "--seed"))          { o.seed = (unsigned)parse_int(a, v, 0, 2147483647); i++; }
        else if (!strcmp(a, "--rows"))     { o.rows = parse_int(a, v, 5, MAZE_MAX_ROWS); i++; }
        else if (!strcmp(a, "--cols"))     { o.cols = parse_int(a, v, 7, MAZE_MAX_COLS); i++; }
        else if (!strcmp(a, "--steps"))    { o.max_steps = parse_int(a, v, 0, 10000000); i++; }
        else if (!strcmp(a, "--delay"))    { o.delay_ms = parse_int(a, v, 0, 10000); i++; }
        else if (!strcmp(a, "--quiet"))    { o.quiet = true; }
        else if (!strcmp(a, "--strategy")) {
            if (v && !strcmp(v, "explore"))     o.strategy = STRATEGY_EXPLORE;
            else if (v && !strcmp(v, "random")) o.strategy = STRATEGY_RANDOM;
            else { fprintf(stderr, "--strategy expects explore or random, got '%s'\n", v ? v : ""); exit(2); }
            i++;
        }
        else if (!strcmp(a, "--help") || !strcmp(a, "-h")) { usage(argv[0]); exit(0); }
        else { fprintf(stderr, "unknown option '%s'\n", a); usage(argv[0]); exit(2); }
    }
    return o;
}

static void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep((useconds_t)ms * 1000);
#endif
}

static void enable_ansi(void)
{
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | 0x0004 /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */);
#endif
}

static char robot_glyph(Heading h)
{
    switch (h) {
    case HEAD_UP:    return '^';
    case HEAD_DOWN:  return 'v';
    case HEAD_LEFT:  return '<';
    default:         return '>';
    }
}

/* Top row printed first, since row 0 is the bottom. */
static void draw(const Maze *m, const Robot *r, int reachable, const char *strategy)
{
    for (int row = m->rows - 1; row >= 0; row--) {
        for (int col = 0; col < m->cols; col++) {
            char ch = ' ';
            if (row == r->pos.row && col == r->pos.col) ch = robot_glyph(r->heading);
            else if (m->cell[row][col])                 ch = '#';
            else if (r->visited[row][col])              ch = '.';
            putchar(ch);
            putchar(ch == '#' ? '#' : ' ');
        }
        putchar('\n');
    }
    printf("strategy: %s  steps: %d  visited: %d/%d reachable nodes\n",
           strategy, r->steps, r->visited_count, reachable);
}

int main(int argc, char **argv)
{
    static Maze maze;
    static Robot robot;
    Options o = parse_args(argc, argv);
    const char *strategy = o.strategy == STRATEGY_EXPLORE ? "explore" : "random";
    Node start = {2, 4};
    int full_at = -1;

    rng_seed(o.seed);
    maze_init(&maze, o.rows, o.cols);
    maze_randomize(&maze);
    maze.cell[start.row][start.col] = 0;

    int reachable = maze_count_reachable(&maze, start);
    robot_init(&robot, start, o.strategy);
    if (!o.quiet)
        enable_ansi();

    while (robot.steps < o.max_steps) {
        if (!o.quiet) {
            printf("\x1b[H\x1b[2J");
            draw(&maze, &robot, reachable, strategy);
            fflush(stdout);
            sleep_ms(o.delay_ms);
        }
        if (robot.visited_count == reachable) {
            full_at = robot.steps;
            break;
        }
        if (!robot_step(&robot, &maze)) {
            printf("robot is boxed in at row %d, col %d\n", robot.pos.row, robot.pos.col);
            break;
        }
    }
    if (full_at < 0 && robot.visited_count == reachable)
        full_at = robot.steps;

    if (o.quiet)
        draw(&maze, &robot, reachable, strategy);
    printf("seed: %u\n", o.seed);
    if (full_at >= 0)
        printf("all %d reachable nodes visited in %d steps\n", reachable, full_at);
    else
        printf("stopped after %d steps with %d/%d nodes visited\n", robot.steps, robot.visited_count, reachable);
    return 0;
}
