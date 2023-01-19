#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include "fs.h"

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s file offset count fs_img\n", basename(argv[0]));
	fprintf(stderr, "Output a file's contents to the standard output.\n");
	fprintf(stderr, "file: file to read and write the contents to stdout.\n");
	fprintf(stderr, "offset: offset where to start reading the file (>= 0).\n");
	fprintf(stderr, "count: number of bytes to read (>= 0).\n");
	fprintf(stderr, "fs: BFS file system image.\n");
	exit(EXIT_FAILURE);
}

/// Application's entry point.
/// @param argc number of command line arguments.
/// @param argv command line arguments.
int main(int argc, char *argv[]) {
	if (argc != 5)
		usage(argv);

	char *path = argv[1];
	int offset = atoi(argv[2]);
	int count = atoi(argv[3]);
	char *fs_img = argv[4];
	
	if (offset < 0 || count < 0)
		usage(argv);

	FILE *fs = fopen(fs_img, "r+");
	if (!fs) {
		fprintf(stderr, "Failed opening \"%s\"!\n", fs_img);
		return EXIT_FAILURE;
	}

	uint8_t buf[count];
	if (!fs_read(basename(path), offset, count, buf, fs))
		return EXIT_FAILURE;
		
	fwrite(buf, count, 1, stdout);
	fclose(fs);

	return EXIT_SUCCESS;
}
