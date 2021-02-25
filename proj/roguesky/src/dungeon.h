#ifndef dungeon_T
#define dungeon_T

#include "common.h"

class island;

//idea: you make a dungeon object based on some config file, then call its methods several times to generate several dungeons on an island
class dungeon {
	int minsize, maxsize;
	int minspace, maxspace;
	int drunken;
	island_tile *legal_tiles;
	box bbox;

	//used in add_tiles, but could be easily removed
	int *buffer = 0;
	size_t buff_len = 0;

	std::vector<box> *rooms;
	std::queue<box> *queue;

	box make_room(int x, int y);
	void enqueue_neighbours(box r);

	void bbox_add(box &bbox, box to_add);
	bool location_clear(box r);
	void generate_room(island *island, box r);
	void connect_rooms(island *island, box room1, box room2);
	void generate_exit(island *island, box room);
public:
	dungeon(int minsize, int maxsize, int minspace, int maxspace, int drunken, island_tile *legal_tiles);
	~dungeon();

	//tries to generate a dungeon onto the island at the coordinates. Returns false if it cannot generate it.
	bool try_generate(island *island, int x, int y);
	//modifies the island to add all the tiles from the dungeon to the island - only call after try_generate succeeds
	void add_tiles(island *island);
};

int dungeon_tilechecker(island_tile t);

#endif
