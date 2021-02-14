#include "dungeon.h"
#include "island.h"

dungeon::dungeon(int minsize, int maxsize, int minspace, int maxspace, island_tile *legal_tiles)
		: minsize(minsize), maxsize(maxsize), minspace(minspace), maxspace(maxspace), guesses(0), rooms(0), legal_tiles(legal_tiles) { }

dungeon::~dungeon() {
	free(this->legal_tiles);
}

struct room dungeon::make_room(int x, int y) {
	return {x, y, this->minsize + randint(this->maxsize - this->minsize + 1), this->minsize + randint(this->maxsize - this->minsize + 1)};
}

bool dungeon::try_generate(island *island, int x, int y) {
	//test the first room
	struct room room = this->make_room(x, y);
	if (island->border_clear(this->legal_tiles, room.x, room.y, room.w, room.h)) {
		fprintf(stderr, "Can probably generate a dungeon here! (%d, %d) (%d, %d)\n", room.x, room.y, room.x+room.w, room.y+room.h);
		this->rooms = new std::vector<struct room>(this->maxsize);
		this->rooms->push_back(room);
		//now we must generate the actual dungeon
		this->queue = new std::queue<struct room>();
		this->enqueue_neighbours(room);
		while (!this->queue->empty() && this->rooms->size() < this->maxsize) {
			
		}
		return true;
	}
	return false;
}

void dungeon::add_tiles(island *island) {
	for (int i = 0; i < this->rooms->size(); i++) {
		struct room r = (*(this->rooms))[i];
		for (int x = r.x; x < r.x + r.w; x++) {
			for (int y = r.y; y < r.y + r.h; y++) {
				(*island)[x][y] = T_AIR;
			}
		}
	}
	delete this->rooms;
}

void dungeon::enqueue_neighbours(struct room r) {
	int seperation, new_val;
	struct room new_room;
	//top neighbour
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.x + randint(r.w);
	new_room = make_room(new_val, r.y - seperation);
	new_room.y -= new_room.h + 1;
	if (randint(2))
		new_room.x -= new_room.w;
	this->queue->push_back(new_room);
	//left
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.y + randint(r.h);
	new_room = make_room(r.x - seperation, new_val);
	new_room.x -= new_room.w + 1;
	if (randint(2))
		new_room.y -= new_room.h;
	this->queue->push_back(new_room);
	//right
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.y + randint(r.h);
	new_room = make_room(r.x + r.w - 1 + seperation, new_val);
	if (randint(2))
		new_room.y -= new_room.h;
	this->queue->push_back(new_room);
	//bottom
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.x + randint(r.w);
	new_room = make_room(new_val, r.y + r.h - 1 + seperation);
	if (randint(2))
		new_room.x -= new_room.w;
	this->queue->push_back(new_room);
}
