#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/// Wrapper around calloc which checks whether the allocation was successful.
/// If it wasn't, the function prints an error message with the offender
/// file and line and exit the program.
/// @param size size in bytes of the chunk to allocate
/// @param file C source file where alloc was called
/// @param line line number where alloc was called
/// @return a pointer to the allocated memory
void *alloc(size_t size, char *file, int line) {
	assert(size > 0);
	assert(file);
	void *p = calloc(1, size);
	if (!p) {
		fprintf(stderr, "Memory allocation failure at file %s, line %d!\n", file, line);
		exit(EXIT_FAILURE);
	}
	return p;
}
