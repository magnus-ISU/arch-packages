#include "pathfind.h"
#include "island.h"

pathfinder::pathfinder(island *island) : isle(island) {
	if (!island)
		throw 0;
	this->heap = new std::priority_queue<pointwd, std::vector<pointwd>, point_comp> ();
	this->steps = new dir_grid(island->w(), island->h());
	this->visited = new compact_bool_matrix(island->w(), island->h());
}

pathfinder::~pathfinder() {
	delete this->heap;
	delete this->steps;
	delete this->visited;
}

path *pathfinder::find(point start, point end, int (*tilechecker)(island_tile)) {
	//special case to avoid clearing the heap and visited
	if (start == end)
		return new path();
	//default pathfinding - avoid all walls, accept all floors equally
	if (!tilechecker)
		tilechecker = default_pathfinding_tilechecker;
	clearpq(*(this->heap));
	this->visited->clear();
	//find from end to start so that way the path is easiest to add
	//We do not set anything in steps because we aren't coming from anywhere // this->steps->set(end, UP);
	this->visited->set(end, 1);
	pointwd next(end, point_distance_squared(start, end));
	this->heap->push(next);
	while (!this->heap->empty()) {
		next = this->heap->top();
		this->heap->pop();
		if (next.p == start)
			goto found;
		//add the four surrounding points to the heap, steps and visited if not already visited
		this->consider(next.p, LEFT, start, tilechecker);
		this->consider(next.p, RIGHT, start, tilechecker);
		this->consider(next.p, UP, start, tilechecker);
		this->consider(next.p, DOWN, start, tilechecker);
	}
	//there is no cleanup because we allocate all the memory upon creating the object! yay
	//This means that we did not find it though... not yay
	return 0;
found:
	path *retval = new path();
	point step(dir2point(start, this->steps->get(start)));
	while (end != step) {
		retval->points.push_back(step);
		step = dir2point(step, this->steps->get(step));
	}
	retval->points.push_back(end);
	return retval;
}

//If a legal point for the path, adds the point p to the heap for consideration
void pathfinder::consider(point from, direction dir, point target, int (*tilechecker)(island_tile)) {
	//change from to the point we actually will consider
	from = dir2point(from, dir);
	//bounds check, because we don't do that anywhere else
	if (from.x >= this->isle->w() || from.x < 0 || from.y < 0 || from.y >= this->isle->h())
		return; //not an error, just means that the point this is from is on the border
	//if already visited, return
	if (this->visited->get(from))
		return;
	//Regardless of whether we decide to go here, we definitely have looked here
	this->visited->set(from, 1);
	//Check if this is a valid tile to walk on - it wouldn't be much of a pathfinder if we could go just anywhere
	int dist = point_distance_squared(from, target);
	island_tile spot = (*this->isle)[from.x][from.y];
	//Get the difficulty of this particular spot
	int tileval = tilechecker(spot);
	if (-1 == tileval)
		return;
	dist += tileval;
	//Since this is a valid spot, we will add it to steps and to heap
	this->steps->set(from, dir_invert(dir));
	this->heap->push(pointwd(from, dist));
}

bool point_comp::operator () (const pointwd &lhs, const pointwd &rhs) {
	return lhs.d > rhs.d;
}

pointwd::pointwd(point p, int d) : p(p), d(d) { }

dir_grid::dir_grid(int w, int h) : isvert(w, h), isneg(w, h) {
	isvert.clear();
	isneg.clear();
}
dir_grid::~dir_grid() { }
direction dir_grid::get(int x, int y) {
	//Uses the spec of direction that isvert * 2 + isneg = direction
	return (direction) (2 * this->isvert.get(x, y) + this->isneg.get(x, y));
}
void dir_grid::set(int x, int y, direction d) {
	//Uses the spec of direction that isvert * 2 + isneg = direction
	this->isvert.set(x, y, (d / 2));
	this->isneg.set(x, y, (d % 2));
}
direction dir_grid::get(point p) {
	return this->get(p.x, p.y);
}
void dir_grid::set(point w, direction d) {
	this->set(w.x, w.y, d);
}

compact_bool_matrix::compact_bool_matrix(int w, int h) : w(w), h(h) {
	this->vals = (byte*) mylloc((w * h + 7) / 8);
}
compact_bool_matrix::~compact_bool_matrix() {
	free(this->vals);
}
bool compact_bool_matrix::get(int x, int y) {
	int bytenum = x + y * this->w;
	int bit = bytenum % 8;
	bytenum /= 8;
	return this->vals[bytenum] & (1 << bit);
}
void compact_bool_matrix::clear() {
	memset(this->vals, 0, (this->w * this->h + 7) / 8);
}
void compact_bool_matrix::set(int x, int y, bool val) {
	int bytenum = x + y * this->w;
	int bit = bytenum % 8;
	byte mask = (byte) ~0;
	if (!val)
		mask = 0;
	bytenum /= 8;
	(this->vals)[bytenum] = ((this->vals)[bytenum] & ~(1 << bit)) | (mask & (1 << bit));
}
bool compact_bool_matrix::get(point p) {
	return this->get(p.x, p.y);
}
void compact_bool_matrix::set(point p, bool val) {
	this->set(p.x, p.y, val);
}

int default_pathfinding_tilechecker(island_tile t) {
	switch (t) {
	case T_FLOOR_CAVE:
	case T_FLOOR_TILE:
	case T_FLOOR_WOOD:
	case T_FLOOR_GRAV:
	case T_FLOOR_PATH:
	case T_FLOOR_GRASS:
		return 0;
	default:
		return -1;
	}
}

point dir2point(point p, direction d) {
	switch (d) {
	case LEFT:
		return point(p.x - 1, p.y);
	case RIGHT:
		return point(p.x + 1, p.y);
	case DOWN:
		return point(p.x, p.y + 1);
	case UP:
		return point(p.x, p.y - 1);
	default:
		throw d;
	}
}

direction dir_invert(direction d) {
	switch (d) {
	case LEFT:
		return RIGHT;
	case RIGHT:
		return LEFT;
	case DOWN:
		return UP;
	case UP:
		return DOWN;
	default:
		throw d;
	}
}
