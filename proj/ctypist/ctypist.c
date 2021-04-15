/*
 * Reads words from standard input and prompts the user to type the words. Calculates the speed at which the words are typed.
 * Escape exits, or finishing all of standard input.
 * If a word is longer than the width, it is skipped.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

typedef unsigned char byte;
typedef const char * col_t;

#define GREEN   "\033[0;32m"
#define RED     "\033[0;31m"
#define YELLOW  "\033[0;33m"
#define WHITE   "\033[0;37m"
#define EMPHON  "\033[1m"
#define EMPHOFF "\033[21m"

#define CURSORUP_PRE "\033["
#define CURSORUP_POST "A"

int output_width = 80;
int buffer_lines = 6;
int errors = 0;
struct timeval start, stop;
struct termios oldt, newt;
char **buffers;
byte *input;
int input_len;
char *next_word;
int next_word_len;
FILE *words;
char *filename = "/usr/share/dict/words";
double secs;
int chars = 0;
int wpm = 0;

byte is_printable_ascii(int c);
byte is_backspace(int c);
byte is_whitespace(int c);
void cleanup(void);
void cleanfree(void);
void startup(void);
void print_typist(void);
void print_lines(void);
void print_header(void);
void parse_args(int argc, char **argv);
void *mylloc(size_t bytes);
void populate_buffer(int index);
void setcol(col_t col);
void get_next_word();

int main(int argc, char **argv) {
	startup();
	int c;
	parse_args(argc, argv);

	**buffers = 0;
	get_next_word();
	for (c = 1; c < buffer_lines; c++)
		populate_buffer(c);

	print_header();
	print_lines();
	c = getchar();
	for (;;) {
		if (c == 27)
			break;
		if (is_whitespace(c)) {
			if (input_len == output_width || buffers[1][input_len] == '\0') {
				//next line
				input_len = 0;
				char *temp = buffers[0];
				for (c = 0; c < buffer_lines - 1; c++)
					buffers[c] = buffers[c+1];
				buffers[buffer_lines - 1] = temp;
				populate_buffer(buffer_lines - 1);
				chars++;
			} else {
				if (buffers[1][input_len] == ' ')
					chars++;
				else
					errors++;
				input[input_len++] = ' ';
			}
		} else if (is_backspace(c)) {
			//do nothing
		} else if (is_printable_ascii(c)) {
			if (input_len == output_width || buffers[1][input_len] == '\0') {
				errors++;
			} else {
				if (c == buffers[1][input_len])
					chars++;
				else
					errors++;
				input[input_len++] = c;
			}
		}
		print_typist();
		c = getchar();
		gettimeofday(&stop, 0);
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		wpm = (int) (chars * 12 / secs);
	}
}

//returns the next word in the rest of argv shorter than width
//if EOF, sets next_word_len to output_width
void get_next_word() {
	int c, i = 0;
	while (EOF != (c = fgetc(words)))
		//consume until printable
		if (!is_whitespace(c))
			break;
	ungetc(c, words);
	while (EOF != (c = fgetc(words))) {
		if (is_whitespace(c)) {
			break;
		} else {
			next_word[i++] = c;
		}
	}
	next_word[i] = 0;
	next_word_len = strlen(next_word);
	if (i == 0)
		next_word_len = output_width;
}

void populate_buffer(int index) {
	int i, linelen = 0;
	while (linelen + next_word_len < output_width) {
		//can't use strcpy because we don't have space for a terminating null
		for (i = 0; next_word[i]; i++)
			buffers[index][linelen + i] = next_word[i];
		buffers[index][linelen + next_word_len] = ' ';
		linelen += i + 1;
		get_next_word();
	}
	buffers[index][linelen - 1] = '\0';
}

void parse_args(int argc, char **argv) {
	int i;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-w")) {
			output_width = atoi(argv[++i]);
		} else if (!strcmp(argv[i], "-h")) {
			buffer_lines = atoi(argv[++i]);
		} else if (!strcmp(argv[i], "-f")) {
			filename = argv[++i];
		} else {
			fprintf(stderr, "Unknown option '%s' encountered\n", argv[i]);
			exit(1);
		}
	}
	if (output_width < 30) {
		fprintf(stderr, "Width should be at least 30\n");
		exit(1);
	}
	if (buffer_lines < 2) {
		fprintf(stderr, "Height should be at least 2\n");
		exit(1);
	}
	buffers = mylloc(sizeof(*buffers) * buffer_lines);
	next_word = mylloc(output_width + 1);
	input = mylloc(output_width);
	input_len = 0;
	for (i = 0; i < buffer_lines; i++)
		buffers[i] = mylloc(output_width);
	words = fopen(filename, "r");
	if (!words) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		exit(1);
	}
	atexit(cleanfree);
}

void print_header(void) {
	int i;
	printf("+");
	for (i = 0; i < output_width - 2; i++)
		printf("-");
	printf("+\n");

	printf("|");
	setcol(YELLOW);
	printf(" WPM: % 5d", wpm);
	for (i = 0; i < output_width - 29; i++)
		printf(" ");
	setcol(RED);
	printf("  ERRORS: % 5d", errors);
	setcol(WHITE);
	printf(" |\n");

	printf("+");
	for (i = 0; i < output_width - 2; i++)
		printf("-");
	printf("+\n");
}

void print_lines(void) {
	int i, j;
	//print first line
	for (i = 0; i < output_width && buffers[0][i]; i++)
		printf("%c", buffers[0][i]);
	for (; i < output_width; i++)
		printf(" ");
	printf("\n");
	setcol(EMPHON);
	for (i = 0; i < output_width && buffers[1][i]; i++) {
		if (i == input_len) {
			setcol(EMPHOFF);
			setcol(WHITE);
		} else if (i < input_len) {
			if (buffers[1][i] == input[i]) {
				setcol(GREEN);
			} else {
				setcol(RED);
			}
		}
		printf("%c", buffers[1][i]);
	}
	for (; i < output_width; i++)
		printf(" ");
	setcol(EMPHOFF);
	setcol(WHITE);
	printf("\n");
	for (j = 2; j < buffer_lines; j++) {
		for (i = 0; i < output_width && buffers[j][i]; i++)
			printf("%c", buffers[j][i]);
		for (; i < output_width; i++)
			printf(" ");
		printf("\n");
	}
}

void print_reset(void) {
	setcol(WHITE);
	setcol(EMPHOFF);
	printf("%s%d%s", CURSORUP_PRE, 3 + buffer_lines, CURSORUP_POST);
}

void print_typist(void) {
	print_reset();
	print_header();
	print_lines();
}

byte is_printable_ascii(int c) {
	return c >= 32 && c < 127;
}
byte is_backspace(int c) {
	return c == 8 || c == 127;
}

void startup(void) {
	tcgetattr(STDIN_FILENO, &oldt);          //Get current terminal settings
	newt = oldt;                             //Allow us to revert to old settings
	newt.c_lflag &= ~(ICANON | ECHO);        //Flush input on every keystroke
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); //Flush input on every keystroke
	printf("\e[?25l");                       //No cursor
	gettimeofday(&start, 0);                 //Time of first key press

	atexit(cleanup);
}

void cleanup(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	printf("\e[?25h");
}

void cleanfree(void) {
	int i;
	for (i = 0; i < buffer_lines; i++) {
		free(buffers[i]);
	}
	free(buffers);
	free(input);
	free(next_word);
	fclose(words);
}

void setcol(col_t col) {
	printf(col);
}

void *mylloc(size_t bytes) {
	void *retval = malloc(bytes);
	if (!retval) {
		fprintf(stderr, "Malloc failed for %lu bytes", bytes);
		exit(1);
	}
	return retval;
}

byte is_whitespace(int c) {
	switch(c) {
	case ' ':
	case '\n':
	case '\r':
	case '\t':
		return 1;
	default:
		return 0;
	}
}
