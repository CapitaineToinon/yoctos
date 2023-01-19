#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include "fs.h"
#include "file.h"

/// Output the specified file's contents to stdout.
/// @param filename name of the file to output
/// @param fs the file system image for which to locate the file
static bool cat_file(char *filename, FILE *fs) {
	superblock_t *sb = superblock_read(fs);
	uint bs = sb->block_size;
	inode_table_t itab = inode_table_read(sb->inode_count, sb->inode_start, bs, fs);
	inode_t *inode = fs_get_file_inode(filename, &itab);
	if (!inode) {
		inode_table_destroy(&itab);
		superblock_destroy(sb);
		fprintf(stderr, "File not found (\"%s\")!\n", filename);
		return false;
	}
	uint count = inode->size;
	uint8_t buf[count];
	bool status = fs_read(filename, 0, count, buf, fs);
	if (status)
		fwrite(buf, count, 1, stdout);
	inode_table_destroy(&itab);
	superblock_destroy(sb);
	return status;
}

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s file fs_img\n", basename(argv[0]));
	fprintf(stderr, "Output a file's contents to the standard output.\n");
	fprintf(stderr, "file: file to output.\n");
	fprintf(stderr, "fs: BFS file system image.\n");
	exit(EXIT_FAILURE);
}

/// Application's entry point.
/// @param argc number of command line arguments.
/// @param argv command line arguments.
int main(int argc, char* argv[]) {
	if (argc != 3)
		usage(argv);

	char *path = argv[1];
	char *fs_img = argv[2];

	FILE *fs = fopen(fs_img, "r+");
	if (!fs) {
		fprintf(stderr, "Failed opening \"%s\"!\n", fs_img);
		return EXIT_FAILURE;
	}

	if (!cat_file(basename(path), fs))
		return EXIT_FAILURE;

	fclose(fs);

	return EXIT_SUCCESS;
}
