#ifndef dungeon_T
#define dungeon_T

#include "common.h"

class island;

//idea: you make a dungeon object based on some config file, then call its methods several times to generate several dungeons
class dungeon {
	int minsize, maxsize;
	int minspace, maxspace;
	island_tile *legal_tiles;

	std::vector<struct box> *rooms;
	std::queue<struct box> *queue;

	struct box make_room(int x, int y);
	void enqueue_neighbours(struct box r);

	void bbox_add(struct box &bbox, struct box to_add);
	bool location_clear(struct box r);
public:
	dungeon(int minsize, int maxsize, int minspace, int maxspace, island_tile *legal_tiles);
	~dungeon();

	//tries to generate a dungeon onto the island at the coordinates. Returns false if it cannot generate it.
	bool try_generate(island *island, int x, int y);
	//modifies the island to add all the tiles from the dungeon to the island - only call after try_generate succeeds
	void add_tiles(island *island);
};

#endif
