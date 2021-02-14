#ifndef ROGUE_HELPERS_H
#define ROGUE_HELPERS_H

#include <stddef.h>
#include <stdbool.h>

// Defines colors for printing to console
#define BACKGROUND_WHITE "\x1b[48;2;255;255;255m"
#define BACKGROUND_GREY "\x1b[48;2;127;127;127m"
#define BACKGROUND_BLACK "\x1b[48;2;0;0;0m"

// Defines the proper key combo to reset the console
#define CONSOLE_RESET "\x1b[0m"

// Contains useful program initializers so main() can change frequently
void init_program();

// Wraps malloc() with an additional check to make sure the pointer is real. If it's not the program kills.
void *safe_malloc(size_t n);

// Same as safe_malloc()
void *safe_realloc(void *ptr, size_t n);

// Returns a random integer in the range [lower, upper] (inclusive). LOWER MUST BE < UPPER OR AN ARITHMETIC FAULT
// WILL BE GENERATED
int rand_int_in_range(int lower, int upper);

// Shuffles the given int array with a Fisher-Yates algorithm. Modifies the array in memory.
void shuffle_int_array(int *arr, int n);

// Wrapper to kill the program while informing the user of why. Used in error handling.
_Noreturn void kill(char *reason);

// Same as above, but with the additional power to include an int (i.e. Failed to allocate a number of bytes).
_Noreturn void kill_with_int(char *reason_start, int number, char *reason_end);

#endif //ROGUE_HELPERS_H
