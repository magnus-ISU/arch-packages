#include "island.h"

island::island(int w, int h) {
	this->width = w;
	this->height = h;
	this->map = (island_tile *) mylloc(w * h * sizeof(*(this->map)));
	generate_map();
}

island::~island() {
	free(this->map);
}

island_tile *island::operator [] (int x) {
	return this->map + x * this->height;
}

island_tile island::operator () (int x, int y) {
	if (x < 0 || x >= this->width) return T_ILLEGAL;
	if (y < 0 || y >= this->height) return T_ILLEGAL;
	return (*this)[x][y];
}

int island::w() {
	return this->width;
}

int island::h() {
	return this->height;
}

#define BIG_SCALE 96
#define LIL_SCALE 16
#define FAR_PENALTY 3
//TODO instead of having a NUM_DUNGEONS have a dungeon density and check regular points on a grid for building a dungeon
#define NUM_DUNGEONS 1000
void island::generate_map() {
	//information about what we generated
	int furthest[4] = {0, 0, this->width, this->height};

	//calculate perlin noise at every point and make an island-blob from it
	int top_x = this->width / 2;
	int top_y = this->height / 2;
	perlin major(randint(1 << 16));
	perlin minor(randint(1 << 16));
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			double major_noise = major.noise((double) x / BIG_SCALE, (double) y / BIG_SCALE, 0.5);
			double minor_noise = minor.noise((double) x / LIL_SCALE, (double) y / LIL_SCALE, 0.5);
			double modifier = distance_squared((double) x, (double) y, (double) top_x, (double) top_y) / (this->width * this->width + this->height * this->height);
			double noise = major_noise * major_noise * minor_noise - FAR_PENALTY * modifier;
			(*this)[x][y] = perlin_to_tile(noise);
			switch ((*this)[x][y]) {
			case T_AIR:
				break;
			default:
				if (x > furthest[0]) furthest[0] = x;
				if (y > furthest[1]) furthest[1] = y;
				if (x < furthest[2]) furthest[2] = x;
				if (y < furthest[3]) furthest[3] = y;
			}
		}
	}
	//pack the island into a smaller array than it initially appeared in.
	int new_width = furthest[0] - furthest[2] + 1;
	int new_height = furthest[1] - furthest[3] + 1;
	for (int x = 0; x < new_width; x++) {
		for (int y = 0; y < new_height; y++) {
			this->map[y + x * new_height] = (*this)[x + furthest[2]][y + furthest[3]];
		}
	}
	this->width = new_width;
	this->height = new_height;
	this->map = (island_tile *) myrealloc(this->map, new_width * new_height * sizeof(*(this->map)));
	//Add dungeons to the island
	//TODO add a master dungeon which contains the exit

	//first calculate the positions to place the dungeons - we need the number of floors and walls to evenly distribute them
	int num_floor = 0, num_wall=0;
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			switch ((*this)[x][y]) {
			case T_GEN_FLOOR:
				num_floor++;
				break;
			case T_GEN_WALL:
				num_wall++;
				break;
			default:
				break;
			}
		}
	}
	int floors_tried = 0, walls_tried = 0;
	int floors_placed = 0, walls_placed = 0;
	dungeon wall_dungeon(8, 14, 1, 4, get_tile_list(1, T_GEN_WALL));
	dungeon floor_dungeon(4, 9, 3, 7, get_tile_list(1, T_GEN_FLOOR));
	dungeon *generator;
	//loop through each floor and wall and, if they are lucky, try to generate a dungeon at them
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			generator = 0;
			switch((*this)[x][y]) {
			case T_GEN_FLOOR:
				if (randint(num_floor - (floors_tried--)) < (NUM_DUNGEONS - floors_placed))
					generator = &floor_dungeon;
				break;
			case T_GEN_WALL:
				if (randint(num_wall - (walls_tried--)) < (NUM_DUNGEONS - walls_placed))
					generator = &wall_dungeon;
				break;
			default:
				break;
			}
			if (generator) {
				bool attempt = generator->try_generate(this, x, y);
				if (attempt)
					generator->add_tiles(this);
			}
		}
	}
}

void island::print_grid() {
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			printf("%c", island_tile_pretty((*this)[x][y]));
		}
		printf("\n");
	}
}

island_tile island::perlin_to_tile(double perlinvalue) {
	if (perlinvalue > 0.10) return T_GEN_WALL;
	if (perlinvalue > 0) return T_GEN_FLOOR;
	if (perlinvalue > -0.01) return T_GEN_FLOAT;

	return T_AIR;
}

bool island::border_clear(island_tile *legal_list, int x, int y, int w, int h) {
	bool found;
	for (int i = x; i < x+w; i++) {
		found = 0;
		for (int j = 0; legal_list[j]; j++) {
			if ((*this)(i, y) == legal_list[j]) {
				found = 1;
				break;
			}
		}
		if (!found) return 0;
		found = 0;
		for (int j = 0; legal_list[j]; j++) {
			if ((*this)(i, y+w) == legal_list[j]) {
				found = 1;
				break;
			}
		}
		if (!found) return 0;
	}
	for (int i = y; i < y+h; i++) {
		found = 0;
		for (int j = 0; legal_list[j]; j++) {
			if ((*this)(x, i) == legal_list[j]) {
				found = 1;
				break;
			}
		}
		if (!found) return 0;
		found = 0;
		for (int j = 0; legal_list[j]; j++) {
			if ((*this)(x+w, i) == legal_list[j]) {
				found = 1;
				break;
			}
		}
		if (!found) return 0;
	}
	return 1;
}

char island_tile_pretty(island_tile t) {
	switch(t) {
	case T_AIR: return ' ';
	case T_GEN_FLOOR: return '|';
	case T_GEN_WALL: return '#';
	case T_GEN_FLOAT: return '-';
	case T_GEN_DUNG: return '.';
	default:
		fprintf(stderr, "Got a %d tile\n", (int) t);
		throw t;
	}
}

island_tile *get_tile_list(int num_tiles, ...) {
	island_tile *retval = (island_tile*) mylloc(num_tiles + 1);
	va_list args;
	va_start(args, num_tiles);
	for (int i = 0; i < num_tiles; i++) {
		retval[i] = (island_tile) va_arg(args, int);
	}
	va_end(args);
	retval[num_tiles] = T_ILLEGAL;
	return retval;
}
