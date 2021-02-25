#include "common.h"
#include "island.h"

#include <ctime>

#include <raylib.h>

void draw() {
	ClearBackground(RAYWHITE);
}

#define TILESIZE 1
void draw_island(island *isle) {
	for (int x = 0; x < isle->w(); x++) {
		for (int y = 0; y < isle->h(); y++) {
			island_tile t = (*isle)[x][y];
			Color color;
			switch(t) {
			case T_AIR: continue;
			case T_GEN_FLOOR: color = GRAY; break;
			case T_GEN_WALL: color = DARKGRAY; break;
			case T_GEN_FLOAT: color = LIGHTGRAY; break;
			case T_GEN_DUNG: color = RED; break;
			case T_FLOOR_TILE: color = MAROON; break;
			case T_FLOOR_PATH: color = ORANGE; break;
			default:
				fprintf(stderr, "Got a %d tile\n", (int) t);
				throw t;
			}
			Rectangle rec = {(float) x * TILESIZE, (float) y * TILESIZE, (float) TILESIZE, (float) TILESIZE};
			DrawRectangleRec(rec, color);
		}
	}
}

#define ISLE_SIZE_X 1000
#define ISLE_SIZE_Y 1000
int main() {
	srand(time(0));
	island island(ISLE_SIZE_X, ISLE_SIZE_Y);

	InitWindow(GetScreenWidth(), GetScreenHeight(), "Raylib example window");
	SetTargetFPS(144);
	SetWindowState(FLAG_FULLSCREEN_MODE);

	while (!WindowShouldClose()) {
		BeginDrawing();
		draw();
		draw_island(&island);
		DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
