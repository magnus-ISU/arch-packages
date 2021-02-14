#ifndef dungeon_T
#define dungeon_T

#include "common.h"

class island;

struct room { int x; int y; int w; int h; };

//idea: you make a dungeon object based on some config file, then call its methods several times to generate several dungeons
class dungeon {
	int minsize, maxsize;
	int minspace, maxspace;
	island_tile *legal_tiles;

	std::vector<struct room> *rooms;
	std::queue<struct room> *guesses;

	struct room make_room(int x, int y);
public:
	dungeon(int minsize, int maxsize, int minspace, int maxspace, island_tile *legal_tiles);
	~dungeon();

	//tries to generate a dungeon onto the island at the coordinates. Returns false if it cannot generate it.
	bool try_generate(island *island, int x, int y);
	//modifies the island to add all the tiles from the dungeon to the island - only call after try_generate succeeds
	void add_tiles(island *island);
};

#endif
