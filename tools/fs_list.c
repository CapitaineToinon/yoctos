#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "superblock.h"
#include "inode.h"

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s fs\n", basename(argv[0]));
	fprintf(stderr, "List all files in a BFS file system image.\n");
	fprintf(stderr, "fs: BFS file system image.\n");
	exit(EXIT_FAILURE);
}

/// List (to stdout) all the files contained in the specified BFS file system image.
/// @param fs file pointer to a BFS file system image.
static void list_files(FILE *fs) {
	superblock_t *sb = superblock_read(fs);
	uint bs = sb->block_size;
	inode_table_t itab = inode_table_read(sb->inode_count, sb->inode_start, bs, fs);

	for (uint i = 0; i < itab.inode_count; i++) {
		inode_t *inode = inode_get(&itab, i);
		if (inode->number > 0) {  // inode numbers > 0 are in use
			printf("\"%s\" (inode %u, %u bytes)\n", inode->name, inode->number, inode->size);
			//inode_display(inode);
		}
	}

	inode_table_destroy(&itab);
	superblock_destroy(sb);
}

/// Application's entry point.
/// @param argc number of command line arguments.
/// @param argv command line arguments.
int main(int argc, char* argv[]) {
	if (argc != 2)
		usage(argv);

	char *fs_img = argv[1];

	FILE *fs = fopen(fs_img, "r");
	if (!fs) {
		fprintf(stderr, "Failed opening \"%s\"!\n", fs_img);
		return EXIT_FAILURE;
	}

	list_files(fs);
	fclose(fs);

	return EXIT_SUCCESS;
}
