#include "Dungeon/dungeon.h"
#include "Helpers/helpers.h"
#include "settings.h"

// All this is is a driver for the underlying header... this entire codebase is designed to carry forward through
// the entire semester, so main() will always contain minimal code. Not going to bother commenting on what its doing
// since it will change often and wee to wee
int main() {
    init_program();
    Dungeon_T *d = new_dungeon(DUNGEON_Y, DUNGEON_X, MIN_NUM_ROOMS,
                               MAX_NUM_ROOMS, PERCENTAGE_ROOM_COVERED);
    print_dungeon(d);
    cleanup_dungeon(d);
}
