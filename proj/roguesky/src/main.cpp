#include "common.h"
#include "island.h"
#include <ctime>

int main() {
	srand(time(0));
//	srand(3);
	island island(1000, 1000);
	island.print_grid();
	pathfinder p(&island);
}
