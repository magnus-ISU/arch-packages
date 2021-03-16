#include <stdio.h>
#include <string.h>

#define TARGET "\"videoId\":\""
#define READTO '"'

int main() {
	size_t index = 0;
	int c;
	while (index < strlen(TARGET)) {
		c = fgetc(stdin);
		if (EOF == c)
			return 1;
		if (TARGET[index++] == c)
			continue;
		index = 0;
	}
	index = 0;
	for (;;) {
		c = fgetc(stdin);
		if (EOF == c)
			return 2;
		if (READTO == c)
			break;
		fprintf(stdout, "%c", c);
	}
	fprintf(stdout, "\n");
	return 0;
}
