#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

#define COLBLUE		"\x1b[35m"
#define COLNORM		"\x1b[0m"

char* readline(FILE *f, unsigned *len);
char only_whitespace(char *test);
int getnumcols();

int main (int argc, char **argv) {
	int termcols = getnumcols();

	int numcols = 4;
	if (argc == 2) numcols = atoi(argv[1]);

	unsigned i = 0;
	unsigned pos = 0;
	unsigned len;
	unsigned maxlen = termcols / numcols - 2;

	while (!feof(stdin)) {
		char *entry = readline(stdin, &len);
		if (only_whitespace(entry)) {
			free(entry);
			continue;
		}

		if (pos == numcols) {
			pos = 0;
			fprintf(stdout, "\n");
		}

		if (len > maxlen + 1)
			entry[maxlen] = 0;

		fprintf(stdout, "%s", entry);
		if (len > maxlen + 1)
			fprintf(stdout, COLBLUE "> " COLNORM);
		else
			for (i = 0; i < maxlen + 2 - len; i++)
				fprintf(stdout, " ");
		pos++;

		free(entry);
	}

	fprintf(stdout, "\n");
}

char only_whitespace(char *test) {
	for (; *test; test++) {
		if (*test == ' ' || *test == '\t' )
			continue;
		return 0;
	}
	return 1;
}

int getnumcols() {
	int x, y;
	initscr();
	getmaxyx(stdscr, y, x);
	endwin();
	return x;
}

char* readline(FILE *f, unsigned *len) {
	char *retval = malloc(64);
	unsigned max = 64;
	*len = 0;
	int c;
	while ((c = fgetc(f)) != '\n') {
		if (c == EOF || !c) break;
		retval[*len] = (char) c;
		if (++(*len) == max) {
			max *= 2;
			retval = realloc(retval, max);
		}
	}
	retval[*len] = 0;
	retval = realloc(retval, *len + 1);
	return retval;
}
