#include "island.h"

island::island(int w, int h) : width(w), height(h), pather(0) {
	this->map = (island_tile *) mylloc(w * h * sizeof(*(this->map)));
	generate_map();
}

island::~island() {
	free(this->map);
	delete this->pather;
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
#define DUNG_SPACING 4
#define DUNG_SPACE 100
//TODO add parameters which get rid of all the above define statements (possibly) and definitely which get rid of the hardcoded two dungeons that it uses later on.
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
	//We need to allocate the pathfinder now that the correct width and height are there
	this->pather = new pathfinder(this);
	//TODO add a master dungeon which contains the exit

	//Add a bunch of other structures to the island
	std::vector<dungeon*> wall_dungeons;
	std::vector<dungeon*> floor_dungeons;
	dungeon wall_dungeon(8, 14, 1, 4, 10, get_tile_list(1, T_GEN_WALL));
	wall_dungeons.push_back(&wall_dungeon);
	dungeon floor_dungeon(4, 9, 3, 7, 45, get_tile_list(1, T_GEN_FLOOR));
	floor_dungeons.push_back(&floor_dungeon);
	dungeon *generator;
	//I know, quadruple nested for, but the idea is that we only want one dungeon in each DUNG_SPACE by DUNG_SPACE square
	for (int dx = 0; dx < this->width / DUNG_SPACE - 1; dx++) {
		for (int dy = 0; dy < this->height / DUNG_SPACING - 1; dy++) {
			for (int x = dx * DUNG_SPACE; x < dx * DUNG_SPACE + DUNG_SPACE; x += DUNG_SPACING) {
				for (int y = dy * DUNG_SPACE; y < dy * DUNG_SPACE + DUNG_SPACE; y += DUNG_SPACING) {
					switch((*this)[x][y]) {
					case T_GEN_FLOOR:
						generator = floor_dungeons[randint(floor_dungeons.size())];
						break;
					case T_GEN_WALL:
						generator = wall_dungeons[randint(wall_dungeons.size())];
						break;
					default:
						generator = 0;
					}
					if (generator) {
						bool attempt = generator->try_generate(this, x, y);
						if (attempt) {
							generator->add_tiles(this);
							//horrible, but breaks us out of the two loops
							x += DUNG_SPACE;
							y += DUNG_SPACE;
						}
					}
				}
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
	case T_FLOOR_TILE: return ',';
	case T_FLOOR_PATH: return 'p';
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
