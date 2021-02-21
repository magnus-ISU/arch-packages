#include "common.h"
#include "island.h"
#include <ctime>

int main(int argc, char **argv) {
	srand(time(0));
	//island island(800, 600);
	island island(1000, 1000);
	island.print_grid();
}
