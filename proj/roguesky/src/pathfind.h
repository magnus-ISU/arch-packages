#ifndef pathfind_H
#define pathfind_H

#include "common.h"

class island;

//each point from start to finish in the path, excluding start
//pathpoints.size == number of times needed to move to get to the end
class path {
public:
	std::vector<point> points;
};

class pointwd {
public:
	pointwd(point p, int d);
	point p;
	int d;
};

//Horrible thing from https://stackoverflow.com/questions/2852140/priority-queue-clear-method which lets me clear a priority queue.
//This is why modern C++ is actually more evil than goto statements
template <class T, class S, class C> void clearpq(std::priority_queue<T, S, C> &q) {
	struct hack : private std::priority_queue<T, S, C> {
		static S &Container(std::priority_queue<T, S, C> &q) {
			return q.*&hack::c;
		}
	};
	hack::Container(q).clear();
}

class point_comp {
public:
	bool operator () (const pointwd &lhs, const pointwd &rhs);
};

class compact_bool_matrix {
	int w, h;
	byte *vals;
public:
	compact_bool_matrix(int w, int h);
	~compact_bool_matrix();
	void set(int x, int y, bool val);
	bool get(int x, int y);
	void set(point p, bool val);
	bool get(point p);
	void clear();
};

//isvertical * 2 + isnegative * 1 = direction
enum direction {
	LEFT = 1,
	RIGHT = 0,
	UP = 3,
	DOWN = 2,
};
point dir2point(point p, direction d);
direction dir_invert(direction d);

class dir_grid {
	compact_bool_matrix isvert;
	compact_bool_matrix isneg;
public:
	dir_grid(int w, int h);
	~dir_grid();
	void set(int x, int y, direction p);
	direction get(int x, int y);
	void set(point where, direction p);
	direction get(point p);
	//void clear(); //We never need to clear it
};

enum path_type {
	PATH_GEN_DUNG,
	PATH_REG,
};

#define START_HEAP_SIZE 1000
class pathfinder {
	island *isle;
	std::priority_queue<pointwd, std::vector<pointwd>, point_comp> *heap;
	dir_grid *steps;
	compact_bool_matrix *visited;

	void consider(point p, direction d, point target, int (*tilechecker)(island_tile), int intoxication);
public:
	pathfinder(island *island);
	~pathfinder();
	path *find(point start, point end, int (*tilechecker)(island_tile));
	path *drunk(point start, point end, int (*tilechecker)(island_tile), int intoxication);
};

int default_pathfinding_tilechecker(island_tile t);

#endif
