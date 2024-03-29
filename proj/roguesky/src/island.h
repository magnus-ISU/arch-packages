#ifndef island_H
#define island_H

#include "common.h"
#include "perlin.h"
#include "dungeon.h"
#include "pathfind.h"

enum island_tile : byte {
	T_ILLEGAL = 0,

	T_AIR,
	T_GEN_FLOOR,
	T_GEN_WALL,
	T_GEN_FLOAT,
	T_GEN_DUNG,

	T_FLOOR_CAVE,
	T_FLOOR_TILE,
	T_FLOOR_WOOD,
	T_FLOOR_GRAV,
	T_FLOOR_PATH,
	T_FLOOR_GRASS,

	T_WALL_CAVE,
	T_WALL_WOOD,
	T_WALL_BRICK,
	T_WALL_TILE,

	T_WALL_SKYSTONE,
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
	pathfinder *pather;

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
