#include <cstdio>
#include <cstdlib>
#include <cstdint>

typedef unsigned char byte;
typedef uint64_t data;
#define BIGGEST_B10_DATA 100000000000000000
// biggest long is       9223372036854775807

void die(const char *reason) {
	fprintf(stderr, "%s\n", reason);
	exit(1);
}
void *mylloc(size_t bytes) {
	void *retval = malloc(bytes);
	if (!retval)
		die("Malloc fail");
	return retval;
}
void *myrealloc(void *ptr, size_t bytes) {
	void *retval = realloc(ptr, bytes);
	if (!retval)
		die("Realloc fail");
	return retval;
}

// numbers are stored as base 100 numbers in strings
// The ones place is stored in the first byte, hundreds place in the next byte, ten-thousands place in the next byte, and so on
// The length of the num array of a bigint is stored in the len variable, and only that amount of memory is valid.
class base100bigint {
	size_t max;
	size_t len;
	data *num;
public:
	base100bigint(unsigned long long n) {
		this->len = 0;
		unsigned long long backup = n;
		while (backup) {
			this->len++;
			backup /= 100;
		}
		this->max = this->len;
		this->num = (data*) mylloc(this->max * sizeof(*this->num));
		for (byte i = 0; n; i++) {
			this->num[i] = n % BIGGEST_B10_DATA;
			n /= BIGGEST_B10_DATA;
		}
	}
	void add_to_self(base100bigint &other) {
		size_t new_len = (this->len > other.len) ? this->len : other.len;
		if (this->max < new_len + 1) {
			this->max = this->max * 2 + 1;
			this->num = (data*) myrealloc(this->num, this->max * sizeof(*this->num));
		}
		bool carry = 0;
		for (size_t i = 0; i < new_len; i++) {
			if (i >= this->len)
				this->num[i] = 0;
			if (i < other.len)
				this->num[i] += other.num[i];
			if (carry) {
				this->num[i] += 1;
				carry = 0;
			}
			if (this->num[i] >= BIGGEST_B10_DATA) {
				this->num[i] -= BIGGEST_B10_DATA;
				carry = 1;
			}
		}
		this->len = new_len;
		if (carry) {
			this->num[this->len] = 1;
			this->len++;
		}
	}
	void print(FILE *f, const char *suffix) {
		for (size_t i = this->len; i-- > 0;) {
			if (i == this->len - 1) {
				fprintf(f, "%lu", (data) this->num[i]);
				continue;
			}
			data m = BIGGEST_B10_DATA;
			data val = this->num[i];
			while (m > 1) {
				m /= 10;
				int printed = val / m;
				fprintf(f, "%d", printed);
				val = val % m;
			}
		}
		fprintf(f, "%s", suffix);
	}
};

int main(int argc, char **argv) {
	if (argc < 3)
		die("Usage: fib count_to print_every");
	long count_to = atol(argv[1]);
	long print_every = atol(argv[2]);

	base100bigint a(0);
	base100bigint b(1);
	base100bigint *sml = &a;
	base100bigint *big = &b;
	base100bigint *temp;

	fprintf(stdout, "0:\t0\n1:\t1\n");
	for (size_t i = 2; i < (size_t) count_to + 1; i++) {
		sml->add_to_self(*big);
		temp = sml;
		sml = big;
		big = temp;
		if (!((i) % print_every) || !((i + 1) % print_every)) {
			fprintf(stdout, "%lu:\t", i);
			big->print(stdout, "\n");
		}
	}
}
