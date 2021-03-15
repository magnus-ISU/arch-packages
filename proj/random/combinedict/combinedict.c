#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *bufstart = 0;
char **wordlist = 0;
size_t numwords = 0;

#define LOWER "abcdefghijklmnopqrstuvwxyz"
#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT "1234567890"

void usage_and_exit() {
	fprintf(stderr, "Usage: combinedict format\n");
	exit(2);
}

char *mystrdup(char *tocpy) {
	char *buf = malloc(strlen(tocpy));
	for (int i = 0; ; i++) {
		switch(tocpy[i]) {
		case 0:
		case '\n':
		case '\r':
			buf[i] = 0;
			return buf;
		default:
			buf[i] = tocpy[i];
		}
	}
}

//TODO dynamically allocated buffer
void parse_format(char *format, char *buf) {
	if (*format == 0) {
		*buf = 0;
		fprintf(stdout, "%s\n", bufstart);
		return;
	}
	if (format[0] == 'W') {
		for (int i = 0; i < numwords; i++) {
			strcpy(buf, wordlist[i]);
			parse_format(format+1, buf + strlen(wordlist[i]));
		}
		return;
	} else if (format[0] == '\\') {
		*buf = *(++format);
	} else if ('L' == *format) {
		for (int i = 0; i < strlen(LOWER); i++) {
			*buf = LOWER[i];
			parse_format(format + 1, buf + 1);
		}
		return;
	} else if ('U' == *format) {
		for (int i = 0; i < strlen(UPPER); i++) {
			*buf = UPPER[i];
			parse_format(format + 1, buf + 1);
		}
		return;
	} else if ('N' == *format) {
		for (int i = 0; i < strlen(DIGIT); i++) {
			*buf = DIGIT[i];
			parse_format(format + 1, buf + 1);
		}
		return;
	} else if ('P' == *format) {
		*buf = 0;
		fprintf(stdout, "%s\n", bufstart);
		--buf;
	} else {
		*buf = *format;
	}
	parse_format(format + 1, buf + 1);
}

int main(int argc, char **argv) {
	if (argc == 1)
		usage_and_exit();
	size_t maxwords = 0;
	char *gotline = 0;
	size_t maxlinesize = 0;
	while (!feof(stdin)) {
		getline(&gotline, &maxlinesize, stdin);
		//resize words if we must
		if (numwords == maxwords) {
			maxwords = maxwords * 2 + 200;
			wordlist = realloc(wordlist, sizeof(*wordlist) * maxwords);
			if(!wordlist) {
				fprintf(stderr, "Malloc failed!\n");
				exit(1);
			}
		}
		wordlist[numwords++] = mystrdup(gotline);
	}
	free(gotline);
	char buf[1024];
	bufstart = buf;
	parse_format(argv[1], buf);
	for (int i = 0; i < numwords; i++) {
		free(wordlist[i]);
	}
	free(wordlist);
}
