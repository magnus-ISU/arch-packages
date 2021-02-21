#include "common.h"

void *mylloc(size_t num_bytes) {
	void *retval = malloc(num_bytes);
	if (!retval) throw bad_malloc(num_bytes);
	return retval;
}

void *myrealloc(void *ptr, size_t num_bytes) {
	void *retval = realloc(ptr, num_bytes);
	if (!retval) throw bad_malloc(num_bytes);
	return retval;
}

bad_malloc::bad_malloc(int num_bytes) {
	this->num_bytes = num_bytes;
}

const char *bad_malloc::what() {
	return "Failure to allocate space from malloc";
}

int randint(int upper_exclusive) {
	return (int) ((unsigned) rand() % upper_exclusive);
}

int randint(int lower_inclusive, int upper_inclusive) {
	return lower_inclusive + randint(upper_inclusive + 1 - lower_inclusive);
}

double distance_squared(double ax, double ay, double bx, double by) {
	return (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
}

bool box_overlap(struct box a, struct box b) {
	if (a.x < b.x) {
		if (a.x + a.w <= b.x)
			return false;
	} else {
		if (b.x + b.w <= a.x)
			return false;
	}
	if (a.y < b.y) {
		if (a.y + a.h <= b.y)
			return false;
	} else {
		if (b.y + b.h <= a.y)
			return false;
	}
	return true;
}
