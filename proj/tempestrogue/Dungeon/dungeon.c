#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "dungeon.h"
#include "dijkstra.h"
#include "../settings.h"
#include "../Helpers/helpers.h"
#include "../Helpers/tree.h"

// Private struct to store partitions. Same as rooms (for now), but separating them makes it easier to extend Room_T
// in the future, and keeps code more readable.
typedef struct Partition_ {
    int y, x, height, width;
} Partition_T;

// Returns a pointer to a new partition. Disparate "initializer" functions are preferred but they can cause overhead
// in cases where heap space is already allocated to them. In this case, partitions are stored in a tree, so a pointer
// works great (see tree.h for details).
static Partition_T *new_partition(int y, int x, int height, int width) {
    Partition_T *partition = safe_malloc(sizeof(Partition_T));
    partition->y = y;
    partition->x = x;
    partition->height = height;
    partition->width = width;
    return partition;
}

// Determines if room parameters are valid in the given dungeon. Since rooms are contained in a partition, only the
// exterior walls need to be checked if they are within one tile of any other room.
static bool is_valid_room(Dungeon_T *d, int y, int x, int height, int width) {
    int i;
    for (i = y - 1; i < y + height + 1; i++) {
        if (d->map[i][x - 1].type != Rock || d->map[i][x + width].type != Rock) {
            return false;
        }
    }
    for (i = x; i < x + width; i++) {
        if (d->map[y - 1][i].type != Rock || d->map[y + height][i].type != Rock) {
            return false;
        }
    }
    return true;
}

// Heavy lifting function that paints corridors. Given the source pair and destination pair, it will call the
// dijkstra functions to create a new cost map from the source. It then starts at the destination and "rolls downhill".
// By starting at the destination and always visiting the cheapest neighbor node, we can generate a shortest path.
// There may be multiple, but it will always go in order of Up, Down, Left, and then Right, if costs are equal.
// As it builds the path, it will paint any rock tiles to be corridors and set the hardness of those to be equal to
// the default hardness in settings.h
static void paint_corridor(Dungeon_T *d, int src_y, int src_x, int dst_y, int dst_x) {
    int **cost;
    int i, j, neighbor_cost;
    Direction_T direction;
    cost = generate_dijkstra_corridor_map(d, src_y, src_x);
    i = dst_y;
    j = dst_x;
    while (i != src_y || j != src_x) {
        direction = Up;
        neighbor_cost = cost[i - 1][j];
        if (neighbor_cost > cost[i + 1][j]) {
            neighbor_cost = cost[i + 1][j];
            direction = Down;
        }
        if (neighbor_cost > cost[i][j - 1]) {
            neighbor_cost = cost[i][j - 1];
            direction = Left;
        }
        if (neighbor_cost > cost[i][j + 1]) {
            direction = Right;
        }
        switch (direction) { // NOLINT(hicpp-multiway-paths-covered)
            case Up:
                i -= 1;
                break;
            case Down:
                i += 1;
                break;
            case Left:
                j -= 1;
                break;
            case Right:
                j += 1;
                break;
        }
        if (d->map[i][j].type == Rock) {
            d->map[i][j].type = Corridor;
            d->map[i][j].hardness = DEFAULT_CELL_TYPE;
        }
    }
    // print_dungeon(d);
    cleanup_cost_map(cost, d->height);
}

// Initializes a dungeon's variables. Sets num_rooms to be zero, and rooms to NULL. This is the function to be sure to
// update if Cell_T is extended.
static void init_dungeon(Dungeon_T *d, int height, int width) {
    int i, j;
    d->height = height;
    d->width = width;
    d->num_rooms = 0;
    d->rooms = NULL;
    d->map = safe_malloc(d->height * sizeof(Cell_T *));
    for (i = 0; i < d->height; i++) {
        d->map[i] = safe_malloc(d->width * sizeof(Cell_T));
        for (j = 0; j < d->width; j++) {
            d->map[i][j].type = DEFAULT_CELL_TYPE;
            d->map[i][j].hardness = DEFAULT_HARDNESS;
            d->map[i][j].immutable = DEFAULT_IMMUTABLE;
        }
    }
}

// Runs along all sises of the dungeon, setting the immutable flag on them so they cannot be changed.
static void generate_dungeon_border(Dungeon_T *d) {
    int i;
    for (i = 1; i < d->height - 1; i++) {
        d->map[i][0].immutable = true;
        d->map[i][d->width - 1].immutable = true;
    }
    for (i = 0; i < d->width; i++) {
        d->map[0][i].immutable = true;
        d->map[d->height - 1][i].immutable = true;
    }
}

// Helper function that iterates through the array, counts all cells that are rooms, and returns if it is more than
// the provided percentage.
static bool is_room_percentage_covered(Dungeon_T *d, float percentage_covered) {
    int i, j, count;
    count = 0;
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            if (d->map[i][j].type == Room) {
                count++;
            }
        }
    }
    return ((count / (double) (d->height * d->width)) > percentage_covered);
}

// Heavy lifting function that partitions the dungeon. If the dimensions of a given partition are outside of
// PERCENTAGE_SPLIT_FORCE of each other, or one of the sides is in range for the allowed sizes for partitions
// it will split along the long side... otherwise it will randomly choose a direction to split along. It splits along
// a valid range, making sure both resultant partitions will be valid according to the minimums in settings.h.
// It then sets the new partitions to be children of itself, and if the children partitions can be split further it
// will recurse into them, splitting them as necessary.
static void partition(Binary_Tree_T *t) {
    int split;
    bool split_horizontal;
    Partition_T *p, *left, *right;
    p = t->data;
    split_horizontal =
            MIN_PARTITION_HEIGHT <= p->height && p->height <= MAX_PARTITION_HEIGHT ? false :
            MIN_PARTITION_WIDTH <= p->width && p->width <= MAX_PARTITION_WIDTH ? true :
            p->height * (1 + PERCENTAGE_SPLIT_FORCE) < (float) p->width ? false :
            p->width * (1 + PERCENTAGE_SPLIT_FORCE) < (float) p->height ? true :
            rand() % 2; // NOLINT(cert-msc50-cpp)
    if (split_horizontal) {
        split = rand_int_in_range(MIN_PARTITION_HEIGHT, p->height - MIN_PARTITION_HEIGHT);
        left = new_partition(p->y, p->x, split, p->width);
        right = new_partition(p->y + split, p->x, p->height - split, p->width);
    } else {
        split = rand_int_in_range(MIN_PARTITION_WIDTH, p->width - MIN_PARTITION_WIDTH);
        left = new_partition(p->y, p->x, p->height, split);
        right = new_partition(p->y, p->x + split, p->height, p->width - split);
    }
    t->left = new_binary_tree(left);
    t->right = new_binary_tree(right);
    if (!(MIN_PARTITION_HEIGHT <= left->height && left->height <= MAX_PARTITION_HEIGHT) ||
        !(MIN_PARTITION_WIDTH <= left->width && left->width <= MAX_PARTITION_WIDTH)) {
        partition(t->left);
    }
    if (!(MIN_PARTITION_HEIGHT <= right->height && right->height <= MAX_PARTITION_HEIGHT) ||
        !(MIN_PARTITION_WIDTH <= right->width && right->width <= MAX_PARTITION_WIDTH)) {
        partition(t->right);
    }
}

// Heavy lifting function that fills partitions with rooms. It only fills partitions that are at the lowest level,
// meaning they have no children. It will try FAILED_ROOM_PLACEMENT times to fill a room with a valid room, randomly
// generating values that are within range for a valid room, and fit in the partition. If it can't generation a room
// it will return false, and the failure will bubble up back to the caller. In the case of new_dungeon() this will
// trigger a dungeon generation failure and it will try and again from scratch. When it does generate a valid room
// it will then allocate space on the dungeons room array, set the parameters for the array, and paint room tiles
// into the dungeon map.
static bool generate_rooms(Dungeon_T *d, Binary_Tree_T *t) {
    int tries, y, x, height, width, i, j;
    Partition_T *p;
    if (t->left != NULL && t->right != NULL) {
        return generate_rooms(d, t->left) && generate_rooms(d, t->right);
    } else {
        tries = 0;
        p = t->data;
        do {
            if (tries > FAILED_ROOM_PLACEMENT) {
                return false;
            }
            y = rand_int_in_range(p->y, p->y + p->height - MIN_ROOM_HEIGHT);
            x = rand_int_in_range(p->x, p->x + p->height - MIN_ROOM_WIDTH);
            height = rand_int_in_range(MIN_ROOM_HEIGHT, p->y + p->height - y);
            width = rand_int_in_range(MIN_ROOM_WIDTH, p->x + p->height - x);
            tries++;
        } while (!is_valid_room(d, y, x, height, width));
        d->num_rooms++;
        d->rooms = safe_realloc(d->rooms, d->num_rooms * sizeof(Room_T));
        d->rooms[d->num_rooms - 1].y = y;
        d->rooms[d->num_rooms - 1].x = x;
        d->rooms[d->num_rooms - 1].height = height;
        d->rooms[d->num_rooms - 1].width = width;
        for (i = y; i < y + height; i++) {
            for (j = x; j < x + width; j++) {
                d->map[i][j].type = Room;
            }
        }
        return true;
    }
}

// Randomizes the hardness across the dungeon, within the range provided in settings.h. Any immutable cells are set to
// be maximum hardness: otherwise the hardness is added to the base, which is controlled by DEFAULT_HARDNESS.
static void randomize_hardness(Dungeon_T *d) {
    int i, j;
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            if (d->map[i][j].immutable == true) {
                d->map[i][j].hardness = INT_MAX;
            } else {
                if (d->map[i][j].type == Rock) {
                    d->map[i][j].hardness += rand_int_in_range(ROCK_HARDNESS_LOWER, ROCK_HARDNESS_UPPER);
                }
            }
        }
    }
}

// Function that iterates through a random permutation of the rooms. Starting at the first room in the permutation
// it will connect it to the next, and so on, until it's connected all of them. The source and destination coordinates
// for paint_corridor() are set to be random points within the room itself for added variability.
static void generate_corridors(Dungeon_T *d) {
    int *shuffle;
    int r, i, j, y, x;
    shuffle = safe_malloc(d->num_rooms * sizeof(int));
    for (r = 0; r < d->num_rooms; r++) {
        shuffle[r] = r;
    }
    shuffle_int_array(shuffle, d->num_rooms);
    for (r = 0; r < d->num_rooms - 1; r++) {
        i = rand_int_in_range(d->rooms[shuffle[r]].y, d->rooms[shuffle[r]].y + d->rooms[shuffle[r]].height - 1);
        j = rand_int_in_range(d->rooms[shuffle[r]].x, d->rooms[shuffle[r]].x + d->rooms[shuffle[r]].width - 1);
        y = rand_int_in_range(d->rooms[shuffle[r + 1]].y,
                              d->rooms[shuffle[r + 1]].y + d->rooms[shuffle[r + 1]].height - 1);
        x = rand_int_in_range(d->rooms[shuffle[r + 1]].x,
                              d->rooms[shuffle[r + 1]].x + d->rooms[shuffle[r + 1]].width - 1);
        paint_corridor(d, i, j, y, x);
    }
    free(shuffle);
}

// Simply places an upward stair and a downwards stair within rooms in the dungeon, and not in the same room.
static void place_stairs(Dungeon_T *d) {
    int r1, r2, i, j;
    r1 = rand_int_in_range(0, d->num_rooms - 1);
    i = rand_int_in_range(d->rooms[r1].y + 1, d->rooms[r1].y + d->rooms[r1].height - 2);
    j = rand_int_in_range(d->rooms[r1].x + 1, d->rooms[r1].x + d->rooms[r1].width - 2);
    d->map[i][j].type = Stair_Up;
    do {
        r2 = rand_int_in_range(0, d->num_rooms - 1);
    } while (r1 == r2);
    i = rand_int_in_range(d->rooms[r2].y + 1, d->rooms[r2].y + d->rooms[r2].height - 2);
    j = rand_int_in_range(d->rooms[r2].x + 1, d->rooms[r2].x + d->rooms[r2].width - 2);
    d->map[i][j].type = Stair_Down;
}

// Generates a new dungeon. It sets up the dungeon struct itself so it can call init_dungeon() and sets up the binary
// tree for the partition() function, seeding the initial partition with one that is set to be a cell smaller on every
// side, since the outer border of the dungeon is immutable (this guarantees rooms will always generate inside the map
// meaning we can be lazier with our checking). It then tries generating rooms according to the given parameters...
// It it makes too little, too many, fails to place a room in a partition, or doesn't cover enough of the map, it will
// retry, until it has failed too many times. When it generates a workable dungeon, it randomizes the hardness across
// the array, connects rooms, places stairs, cleans up after itself, and returns a pointer to the full dungeon to the
// caller.
Dungeon_T *
new_dungeon(int height, int width, int min_rooms, int max_rooms, float percentage_covered) {
    int tries;
    bool success_room;
    Dungeon_T *d;
    Binary_Tree_T *t;
    d = safe_malloc(sizeof(Dungeon_T));
    t = new_binary_tree(new_partition(1, 1, height - 2, width - 2));
    tries = 0;
    init_dungeon(d, height, width);
    generate_dungeon_border(d);
    do {
        if (tries > FAILED_DUNGEON_GENERATION) {
            kill_with_int("FATAL ERROR! FAILED TO GENERATE WORKABLE DUNGEON AFTER", FAILED_DUNGEON_GENERATION,
                          "TRIES! TRY NEW PARAMETERS!");
        }
        partition(t);
        success_room = generate_rooms(d, t);
        tries++;
    } while (!success_room || !is_room_percentage_covered(d, percentage_covered) ||
             min_rooms > d->num_rooms || d->num_rooms > max_rooms);
    randomize_hardness(d);
    generate_corridors(d);
    place_stairs(d);
    cleanup_binary_tree(t);
    return d;
}

// Cleans up a dungeon, freeing all child data points.
void cleanup_dungeon(Dungeon_T *d) {
    int i;
    for (i = 0; i < d->height; i++) {
        free(d->map[i]);
    }
    free(d->map);
    free(d->rooms);
    free(d);
}

// See dungeon.h
char cell_type_char(Cell_Type_T c) {
    switch (c) {
        case Rock: // NOLINT(bugprone-branch-clone)
            return ' ';
        case Room:
            return ' ';
        case Corridor:
            return ' ';
        case Stair_Up:
            return '<';
        case Stair_Down:
            return '>';
        default:
            return '?';
    }
}

// See dungeon.h
char *cell_type_color(Cell_Type_T c) {
    switch (c) {
        case Rock:
            return BACKGROUND_WHITE;
        case Room:
            return BACKGROUND_BLACK;
        case Corridor:
            return BACKGROUND_GREY;
        case Stair_Up: // NOLINT(bugprone-branch-clone)
            return BACKGROUND_BLACK;
        case Stair_Down:
            return BACKGROUND_BLACK;
        default:
            return CONSOLE_RESET;
    }
}

// See dungeon.h
void print_dungeon(Dungeon_T *d) {
    int i, j;
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            printf("%s%c", cell_type_color(d->map[i][j].type), cell_type_char(d->map[i][j].type));
        }
        printf("%s\n", CONSOLE_RESET);
    }
}



