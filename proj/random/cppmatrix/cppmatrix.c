#include <stdio.h>
#include <stdlib.h>

#include<ncurses.h>

typedef unsigned char byte;

#define NUM_SLIDERS 2

struct col {
	int last_char[NUM_SLIDERS];
	short char_pos[NUM_SLIDERS];
	short void_pos[NUM_SLIDERS];
	byte is_bold[NUM_SLIDERS];
};

struct matrix {
	short nrows;
	short ncols;

	struct col *cols;
};

void curses_start();
void init_matrix(struct matrix *m);
int randint(int upper_bound);
int subtract_random(int from);
int matrix_char();
void print_matrix(struct matrix *m);
void print_matrix_col (struct matrix *m, struct col *c, short colnum);

int main() {
	curses_start();
	struct matrix m = {0};
	int c = KEY_RESIZE;

	for (;;) {
		switch (c) {
			case 'Z':
				exit(0);
			case KEY_RESIZE:
				init_matrix(&m);
				break;

		}
		print_matrix(&m);
		c = getch();
	}
}

void init_matrix (struct matrix *m) {
	free(m->cols);
	refresh();

	getmaxyx(stdscr, m->nrows, m->ncols);

	m->cols = (struct col *) malloc(sizeof(*(m->cols) ) * m->ncols);
	for (short i = 0; i < m->ncols; i++) {
		int starting_line = 0;
		for (short j = 0; j < NUM_SLIDERS; j++) {
			starting_line = subtract_random(subtract_random(starting_line));
			m->cols[i].char_pos[j] = starting_line;
			m->cols[i].is_bold[j]=randint(2);
			starting_line = subtract_random(starting_line);
			m->cols[i].void_pos[j] = starting_line;
		}
	}
}

void print_matrix(struct matrix *m) {
	for (short i = 0; i < m->ncols; i++) {
		print_matrix_col(m, &(m->cols[i]), i);
	}
}

void print_matrix_col (struct matrix *m, struct col *c, short colnum) {
	short min_y = 0;

	//print the previous character in green
	attron(COLOR_PAIR(1));
	for (short i = 0; i < NUM_SLIDERS; i++) {
		if (min_y > c->char_pos[i] ) min_y = c->char_pos[i];
		if (min_y > c->void_pos[i] ) min_y = c->void_pos[i];

		if (c->char_pos[i] >= 0) {
			mvaddch(c->char_pos[i], colnum, c->last_char[i]);
		}
	}
	attroff(COLOR_PAIR(1));

	//print the next character in white, or jump back to the top
	//print a space also
	for (short i = 0; i < NUM_SLIDERS; i++) {
		c->char_pos[i]++;
		if (c->char_pos[i] == m->nrows) {
			c->char_pos[i] = subtract_random(subtract_random(min_y));
		} else if (c->char_pos[i] >= 0) {
			//calculate a new character
			c->last_char[i] = matrix_char();
			if (c->is_bold[i]) c->last_char[i] |= A_BOLD;
			if (!randint(5) ) c->is_bold[i] = !c->is_bold[i];

			mvaddch(c->char_pos[i], colnum, c->last_char[i]);
		}

		c->void_pos[i]++;
		if (c->void_pos[i] == m->nrows) {
			c->void_pos[i] = subtract_random(min_y);
		} else if (c->void_pos[i] >= 0) {
			mvaddch(c->void_pos[i], colnum, ' ');
		}
	}

}

const char *matrixchars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*(){}[]',.`/?=+-_;:\"\\|";
#define len_matrixchars 91
int matrix_char() {
	return matrixchars[randint(len_matrixchars)];
}

int subtract_random(int from) {
	return from - 10 - randint(80);
}

void curses_end() {
	endwin();
}

void curses_start() {
	atexit(curses_end);
	initscr();
	halfdelay(1);
	noecho();
	curs_set(0);

	start_color();
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
}

int randint(int upper_bound) {
	return rand() % upper_bound;
}
