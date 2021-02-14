#include <limits.h>
#include <stdlib.h>

#include "dijkstra.h"
#include "../settings.h"
#include "../Helpers/helpers.h"

// Cost function for the below function. Use parameters in settings.h if you want to tune this.
static int corridor_cost(Dungeon_T *d, int y, int x) {
    if (d->map[y][x].immutable == true) {
        return INT_MAX;
    }
    switch (d->map[y][x].type) {
        case Rock:
            return USE_HARDNESS_FOR_CORRIDORS ? d->map[y][x].hardness + 1 : ROCK_WEIGHT;
        case Stair_Up:
        case Stair_Down:
        case Room:
            return ROOM_WEIGHT;
        case Corridor:
            return CORRIDOR_WEIGHT;
        default:
            return INT_MAX;
    }
}

// Recursive function that does the heavy lifting for generating the corridor cost map. It keeps track of what
// neighbor called it, not entering into that cell to avoid an infinite recursion loop and blowing the stack. It then
// adjusts the cost if it found a cheaper path, and will recurse into its neighbor, otherwise it skips that cells,
// knowing that its path is a dead end.
static void corridor_cost_recurse(Dungeon_T *d, int **cost, int y, int x, Direction_T caller) {
    int neighbor_cost;
    bool up, down, left, right;
    up = down = left = right = false;
    if (caller != Up) {
        if (y + 1 < d->height - 1) {
            neighbor_cost = corridor_cost(d, y + 1, x);
            if (cost[y + 1][x] > cost[y][x] + neighbor_cost) {
                cost[y + 1][x] = cost[y][x] + neighbor_cost;
                down = true;
            }
        }
    }
    if (caller != Down) {
        if (y - 1 > 0) {
            neighbor_cost = corridor_cost(d, y - 1, x);
            if (cost[y - 1][x] > cost[y][x] + neighbor_cost) {
                cost[y - 1][x] = cost[y][x] + neighbor_cost;
                up = true;
            }
        }
    }
    if (caller != Left) {
        if (x + 1 < d->width - 1) {
            neighbor_cost = corridor_cost(d, y, x + 1);
            if (cost[y][x + 1] > cost[y][x] + neighbor_cost) {
                cost[y][x + 1] = cost[y][x] + neighbor_cost;
                right = true;
            }
        }
    }
    if (caller != Right) {
        if (x - 1 > 0) {
            neighbor_cost = corridor_cost(d, y, x - 1);
            if (cost[y][x - 1] > cost[y][x] + neighbor_cost) {
                cost[y][x - 1] = cost[y][x] + neighbor_cost;
                left = true;
            }
        }
    }
    if (up) {
        corridor_cost_recurse(d, cost, y - 1, x, Up);
    }
    if (down) {
        corridor_cost_recurse(d, cost, y + 1, x, Down);
    }
    if (left) {
        corridor_cost_recurse(d, cost, y, x - 1, Left);
    }
    if (right) {
        corridor_cost_recurse(d, cost, y, x + 1, Right);
    }
}

// First it initializes the cost map, setting all values to INT_MAX (infinite effectively). Then it sets the source to
// be zero. It then initializes the recursive function, by figuring out if it can go up, down, left, and right from
// itself, based on if the cells are still in range. To do so, it sets the costs of all the cells it can visit next,
// then recurses into them.
int **generate_dijkstra_corridor_map(Dungeon_T *d, int y, int x) {
    int **cost;
    int i, j;
    bool up, down, left, right;
    cost = safe_malloc(d->height * sizeof(int *));
    for (i = 0; i < d->height; i++) {
        cost[i] = safe_malloc(d->width * sizeof(int));
        for (j = 0; j < d->width; j++) {
            cost[i][j] = INT_MAX;
        }
    }
    cost[y][x] = 0;
    up = y - 1 > 0;
    down = y + 1 < d->height - 1;
    left = x - 1 > 0;
    right = x + 1 < d->width - 1;
    if (up) {
        cost[y - 1][x] = corridor_cost(d, y - 1, x);
    }
    if (down) {
        cost[y + 1][x] = corridor_cost(d, y + 1, x);
    }
    if (left) {
        cost[y][x - 1] = corridor_cost(d, y, x - 1);
    }
    if (right) {
        cost[y][x + 1] = corridor_cost(d, y, x + 1);
    }
    if (up) {
        corridor_cost_recurse(d, cost, y - 1, x, Up);
    }
    if (down) {
        corridor_cost_recurse(d, cost, y + 1, x, Down);
    }
    if (left) {
        corridor_cost_recurse(d, cost, y, x - 1, Left);
    }
    if (right) {
        corridor_cost_recurse(d, cost, y, x + 1, Right);
    }
    return cost;
}

// See dijkstra.h. Self explanatory really.
void cleanup_cost_map(int **cost, int height) {
    int i;
    for (i = 0; i < height; i++) {
        free(cost[i]);
    }
    free(cost);
}
