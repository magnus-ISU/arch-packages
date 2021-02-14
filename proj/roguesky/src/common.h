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
double distance_squared(double ax, double ay, double bx, double by);

enum island_tile : byte;
#endif
