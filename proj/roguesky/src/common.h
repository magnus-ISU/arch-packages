#ifndef types_H
#define types_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

#include <exception>
#include <string>
#include <queue>

typedef unsigned char byte;

// allocates num_bytes and ensures that it was successful, or throws an alloc_error if not
void *mylloc(size_t num_bytes);
void *myrealloc(void *ptr, size_t num_bytes);

class bad_malloc : std::exception
{
public:
	int num_bytes;
	bad_malloc(int num_bytes);
	const char *what();
};

int randint(int upper_exclusive);
int randint(int lower_inclusive, int upper_inclusive);

int *range(int upper_exclusive); // like in python
void shuffle(int *arr, size_t len);

struct box {
public:
	box();
	box(int x, int y, int w, int h);
	int x, y;
	int w, h;
};
bool box_overlap(struct box a, struct box b);

struct point {
public:
	int x, y;

	point(int x, int y);
	bool operator == (const point &p);
	bool operator != (const point &p);
};
int point_distance_squared(point a, point b);
double distance_squared(double ax, double ay, double bx, double by);

enum island_tile : byte;
#endif
