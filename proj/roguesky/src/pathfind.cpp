#include "pathfind.h"
#include "island.h"

pathfinder::pathfinder(island *island) : isle(island) {
	if (!island)
		throw 0;
	this->heap = new std::priority_queue<pointwd, std::vector<pointwd>, point_comp> ();
	this->steps = new point_grid(island->w(), island->h());
	this->visited = new compact_bool_matrix(island->w(), island->h());
}

pathfinder::~pathfinder() {
	delete this->heap;
	delete this->steps;
	delete this->visited;
}

//find from end to start so that way the path is easiest to add
path *pathfinder::drunk(point start, point end, int (*tilechecker)(island_tile), int intoxication) {
	//special case to avoid clearing the heap and visited
	if (start == end)
		return new path();
	//default pathfinding - avoid all walls, accept all floors equally
	if (!tilechecker)
		tilechecker = default_pathfinding_tilechecker;
	clearpq(*(this->heap));
	this->visited->clear();
	this->steps->set(end, end);
	this->visited->set(end, 1);
	pointwd next(end, point_distance_squared(start, end));
	this->heap->push(next);
	while (!this->heap->empty()) {
		next = this->heap->top();
		this->heap->pop();
		if (next.p == start)
			goto found;
		//add the four surrounding points to the heap, steps and visited if not already visited
		this->consider(point(next.p.x - 1, next.p.y), next.p, start, tilechecker, intoxication);
		this->consider(point(next.p.x + 1, next.p.y), next.p, start, tilechecker, intoxication);
		this->consider(point(next.p.x, next.p.y - 1), next.p, start, tilechecker, intoxication);
		this->consider(point(next.p.x, next.p.y + 1), next.p, start, tilechecker, intoxication);
	}
	//there is no cleanup because we allocate all the memory upon creating the object! yay
	//This means that we did not find it though... not yay
	return 0;
found:
	//next is now on start, read steps[start] all the way back to where steps[i] == end
	//we are guaranteed at least 1 value because of the check at the start
	path *retval = new path();
	point step(start);
	do {
		step = this->steps->get(step);
		retval->points.push_back(step);
	} while (end != step);
	return retval;
}

//If a legal point for the path, adds the point p to the heap for consideration
void pathfinder::consider(point p, point wherefrom, point target, int (*tilechecker)(island_tile), int intoxication) {
	//bounds check, because we don't do that anywhere else
	if (p.x >= this->isle->w() || p.x < 0 || p.y < 0 || p.y >= this->isle->h())
		return; //not an error, just means that the point this is from is on the border
	//if already visited, return
	if (this->visited->get(p))
		return;
	//Regardless of whether we decide to go here, we definitely have looked here
	this->visited->set(p, 1);
	//Check if this is a valid tile to walk on - it wouldn't be much of a pathfinder if we could go just anywhere
	int dist = point_distance_squared(p, target);
	dist += randint(intoxication);
	island_tile spot = (*(this->isle))[p.x][p.y];
	//this is really not a good system, probably instead of passing a ptype we should pass a function pointer which takes an island_tile and spits out -1 for illegal or a positive number to add to dist
	//where 0 does a default behaviour which returns 0 for pathable floor tiles and -1 for all others
	int tileval = tilechecker(spot);
	if (-1 == tileval)
		return;
	dist += tileval;
	//Since this is a valid spot, we will add it to steps and to heap
	this->steps->set(p, wherefrom);
	this->heap->push(pointwd(p, dist));
}

path *pathfinder::find(point start, point end, int (*tilechecker)(island_tile)) {
	return this->drunk(start, end, tilechecker, 1);
}

bool point_comp::operator () (const pointwd &lhs, const pointwd &rhs) {
	return lhs.d > rhs.d;
}

pointwd::pointwd(point p, int d) : p(p), d(d) { }

point_grid::point_grid(int w, int h) : w(w), h(h) {
	this->grid = (point*) malloc(sizeof(*(this->grid)) * w * h);
}
point_grid::~point_grid() {
	free(this->grid);
}
point point_grid::get(int x, int y) {
	int index = x + y * this->w;
	return this->grid[index];
}
void point_grid::set(int x, int y, point p) {
	int index = x + y * this->w;
	this->grid[index] = p;
}
point point_grid::get(point p) {
	return this->get(p.x, p.y);
}
void point_grid::set(point w, point p) {
	this->set(w.x, w.y, p);
}

compact_bool_matrix::compact_bool_matrix(int w, int h) : w(w), h(h) {
	this->vals = (byte*) malloc((w * h + 7) / 8);
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
