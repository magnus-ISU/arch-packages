#ifndef island_H
#define island_H

#include "common.h"
#include "perlin.h"
#include "dungeon.h"

enum island_tile : byte {
	T_ILLEGAL = 0,

	T_AIR,
	T_GEN_FLOOR,
	T_GEN_WALL,
	T_GEN_FLOAT,
	T_GEN_DUNG,

	T_CAVE_FLOOR,
	T_TILE_FLOOR,
	T_WOOD_FLOOR,
	T_GRAV_FLOOR,
	T_PATH_FLOOR,
	T_GRASS_FLOOR,

	T_CAVE_WALL,
	T_WOOD_WALL,
	T_BRICK_WALL,

	T_SKYSTONE,
};
//returns an ascii representation of the tile
char island_tile_pretty(island_tile t);
//returns a T_ILLEGAL (null) - terminated list with all the tile types, allocated from malloc - not sure if needed
island_tile *get_tile_list(int num_tiles, ...);

class island {
	int width;
	int height;
	island_tile *map;

	void generate_map();
	island_tile perlin_to_tile(double perlinvalue);
	bool add_dungeon(dungeon *d);
public:
	island(int w, int h);
	~island();
	//faster
	island_tile *operator [] (int x);
	//bounds checking
	island_tile operator () (int x, int y);

	void print_grid();
	//returns if a room can be built here based on the legal_list
	bool border_clear(island_tile *legal_list, int x, int y, int w, int h);

	int w();
	int h();
};

#endif
