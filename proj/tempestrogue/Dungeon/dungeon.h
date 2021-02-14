#ifndef ROGUE_DUNGEON_H
#define ROGUE_DUNGEON_H

#include <stdbool.h>
#include <stddef.h>

// Typedef for possible neighbor directions. Not necessary, but helps avoid programming errors.
typedef enum Direction_E {
    Up, Down, Left, Right
} Direction_T;

// Enum to store the cell type. Allows us to easily add new cell types later if we so desire, and makes code more
// readable and reliable. Make sure to add the new types to cell_type_char() and cell_type_color()
typedef enum Cell_Type_E {
    Rock, Room, Corridor, Stair_Up, Stair_Down
} Cell_Type_T;

// Stores attributes of a cell inside of the Dungeon map. Extensible as long as the corresponding initializer in
// dungeon.c is updated (init_dungeon()).
typedef struct Cell_S {
    Cell_Type_T type;
    int hardness;
    bool immutable;
} Cell_T;

// Stores attributes about a room. y and x refer to the top left point.
typedef struct Room_S {
    int y, x, height, width;
} Room_T;

// Stores all attributes about a dungeon. Will be expanded upon later as new features are added. Extensible as long as
// init_dungeon() and cleanup_dungeon() is updated.
typedef struct Dungeon_S {
    Cell_T **map;
    Room_T *rooms;
    int height, width, num_rooms;
} Dungeon_T;

// Returns a pointer to a new dungeon. A few parameters are able to changed at runtime if the user so desires. For now
// the driver code uses the defaults in settings.h
Dungeon_T *
new_dungeon(int height, int width, int min_rooms, int max_rooms, float percentage_covered);

// Cleans up a dungeon, freeing all child structs and arrays.
void cleanup_dungeon(Dungeon_T *d);

// Returns the char type of a cell type.
char cell_type_char(Cell_Type_T c);

// Returns the ASCI control string to color a cell type.
char *cell_type_color(Cell_Type_T c);

// Iterates through the dungeon and prints it out. Calls cell_type_char() and cell_type_color() to print it.
void print_dungeon(Dungeon_T *d);

#endif //ROGUE_DUNGEON_H
