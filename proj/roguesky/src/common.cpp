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

int *range(int upper_exclusive) {
	int *retval = (int*) mylloc(upper_exclusive * sizeof(*retval));
	for (int i = 0; i < upper_exclusive; i++) {
		retval[i] = i;
	}
	return retval;
}

void shuffle(int *arr, size_t len) {
	int temp;
	for (size_t i = 0; i < len - 1; i++) {
		size_t j = i + randint(len - i);
		temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
}

double distance_squared(double ax, double ay, double bx, double by) {
	return (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
}
int point_distance_squared(point a, point b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

bool box_overlap(box a, box b) {
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

box::box() { }
box::box(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) { }

point::point(int x, int y) : x(x), y(y) { }
point::point(box b) {
	this->x = b.x + randint(b.w);
	this->y = b.y + randint(b.h);
}
bool point::operator == (const point &p) {
	return this->x == p.x && this->y == p.y;
}
bool point::operator != (const point &p) {
	return this->x != p.x || this->y != p.y;
}
