#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foundit(char *target, char *buffer, size_t len, size_t pos) {
	size_t buf_i = pos;
	size_t tar_i = 0;
	for (; tar_i < len; tar_i++) {
		if (buffer[buf_i] != target[tar_i])
			return 0;
		buf_i++;
		if (buf_i == len)
			buf_i = 0;
	}
	return 1;
}

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Finds a string in stdin and reports how many characters in it is\nUsage: findstring targetstring\n");
		exit(2);
	}
	char *target = argv[1];
	size_t len = strlen(target);
	char *buffer = malloc(len);
	size_t pos = 0;
	//fill the buffer
	unsigned long long i = 0;
	while (i < len)
		buffer[i++] = fgetc(stdin);
	//read values until we find it
	for (i = 0; !foundit(target, buffer, len, pos); i++) { //maximum length we can accurately report location is maximum unsigned long long
		int read_char = fgetc(stdin);
		if (EOF == read_char) {
			fprintf(stderr, "Error: given string '%s' is not in the input file, searched %llu strings\n", target, i);
			exit(1);
		}
		buffer[pos++] = (char) read_char;
		if (pos == len)
			pos = 0;
	}
	fprintf(stdout, "'%s' found %llu strings into the input\n", target, ++i);
}
