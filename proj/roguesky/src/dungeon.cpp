#include "dungeon.h"
#include "island.h"

island_tile generation_bounding_box_checker[] = {T_AIR, T_GEN_WALL, T_GEN_FLOOR, T_GEN_FLOAT, T_ILLEGAL};

dungeon::dungeon(int minsize, int maxsize, int minspace, int maxspace, island_tile *legal_tiles)
		: minsize(minsize), maxsize(maxsize), minspace(minspace), maxspace(maxspace), legal_tiles(legal_tiles), rooms(0), queue(0) { }

dungeon::~dungeon() {
	free(this->legal_tiles);
}

bool dungeon::try_generate(island *island, int x, int y) {
	//test the first room, don't make a stack or queue if we don't need to
	struct box room = this->make_room(x, y);
	struct box bbox = room;
	if (island->border_clear(this->legal_tiles, room.x, room.y, room.w, room.h)) {
		unsigned target_size = (unsigned) randint(4, 10);
		this->rooms = new std::vector<struct box>();
		this->rooms->push_back(room);
		this->queue = new std::queue<struct box>();
		//now we must generate the actual dungeon
		enqueue_neighbours(room);
		while (!this->queue->empty() && this->rooms->size() < target_size) {
			room = this->queue->front();
			if (island->border_clear(this->legal_tiles, room.x - this->minspace, room.y - this->minspace, room.w + 2 * this->minspace, room.h + 2 * this->minspace)
					&& this->location_clear(room)) {
				this->rooms->push_back(room);
				bbox_add(bbox, room);
				enqueue_neighbours(room);
			}
			this->queue->pop();
		}
		delete this->queue;
		if (island->border_clear(generation_bounding_box_checker, bbox.x - 1 - this->minspace, bbox.y - 1 - this->minspace, bbox.w + 2 + this->minspace * 2, bbox.h + 2 + this->minspace * 2))
		if (this->rooms->size() >= target_size)
			return true;
		delete this->rooms;
	}
	return false;
}

void dungeon::add_tiles(island *island) {
	//add dungeon tiles for each room
	for (unsigned i = 0; i < this->rooms->size(); i++) {
		struct box r = (*(this->rooms))[i];
		for (int x = r.x; x < r.x + r.w; x++) {
			for (int y = r.y; y < r.y + r.h; y++) {
				(*island)[x][y] = T_GEN_DUNG;
			}
		}
	}
	//add paths between rooms

	//add a number of exits

	delete this->rooms;
}

struct box dungeon::make_room(int x, int y) {
	return {x, y, randint(this->minsize, this->maxsize), randint(this->minsize, this->maxsize)};
}

void dungeon::enqueue_neighbours(struct box r) {
	int seperation, new_val;
	struct box new_room;
	//top neighbour
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.x + randint(r.w);
	new_room = make_room(new_val, r.y - seperation);
	new_room.y -= new_room.h;
	if (randint(2))
		new_room.x -= new_room.w - 1;
	this->queue->push(new_room);
	//left
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.y + randint(r.h);
	new_room = make_room(r.x - seperation, new_val);
	new_room.x -= new_room.w;
	if (randint(2))
		new_room.y -= new_room.h - 1;
	this->queue->push(new_room);
	//right
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.y + randint(r.h);
	new_room = make_room(r.x + r.w + seperation, new_val);
	if (randint(2))
		new_room.y -= new_room.h - 1;
	this->queue->push(new_room);
	//bottom
	seperation = randint(this->minspace, this->maxspace);
	new_val = r.x + randint(r.w);
	new_room = make_room(new_val, r.y + r.h + seperation);
	if (randint(2))
		new_room.x -= new_room.w - 1;
	this->queue->push(new_room);
}

bool dungeon::location_clear(struct box r) {
	r.x -= this->minspace;
	r.y -= this->minspace;
	r.w += this->minspace * 2;
	r.h += this->minspace * 2;
	for (unsigned i = 0; i < this->rooms->size(); i++) {
		if (box_overlap(r, (*(this->rooms))[i]))
			return false;
	}
	return true;
}

void dungeon::bbox_add(struct box &b, struct box a) {
	if (a.x < b.x) {
		b.w += (b.x - a.x);
		b.x = a.x;
	}
	if (a.y < b.y) {
		b.h += (b.y - a.y);
		b.y = a.y;
	}
	if (a.x + a.w > b.x + b.w) {
		b.w = a.x + a.w - b.x;
	}
	if (a.y + a.h > b.y + b.h) {
		b.h = a.y + a.h - b.y;
	}
}
