#ifndef ROGUE_DIJKSTRA_H
#define ROGUE_DIJKSTRA_H

#include "dungeon.h"

// See dijkstra.c for several helper functions.

// Generates a Dijkstra cost map to map corridors. By then "rolling" downhill from the goal to the source, we can
// generate the shortest path and paint a new corridor. See paint_corridor in dungeon.c for details.
int **generate_dijkstra_corridor_map(Dungeon_T *d, int y, int x);

// Frees the 2d array since it is dynamically allocated.
void cleanup_cost_map(int **cost, int height);

#endif //ROGUE_DIJKSTRA_H
