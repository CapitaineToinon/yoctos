#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
#include "blocks.h"

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s label bs inode_count block_count fs\n", basename(argv[0]));
	fprintf(stderr, "Create a BFS file system image.\n");
	fprintf(stderr, "label: label of the file system (alphanumeric chars only, max %d chars).\n", SUPERBLOCK_LABEL_LENGTH-1);
	fprintf(stderr, "bs: data block size in bytes (multiple of 512).\n");
	fprintf(stderr, "inode_count: number of inodes (within [1,65535]); note that one inode will\n");
	fprintf(stderr, "             be reserved by the system and thus unavailable to the user.\n");
	fprintf(stderr, "block_count: number of data blocks (> 0); note that one block will be\n");
	fprintf(stderr, "             reserved by the system and thus unavailable to the user.\n");
	fprintf(stderr, "fs: BFS file system image to create.\n");
	exit(EXIT_FAILURE);
}

/// Application's entry point.
/// @param argc number of command line arguments.
/// @param argv command line arguments.
int main(int argc, char* argv[]) {
	if (argc != 6)
		usage(argv);

	char *label = argv[1];
	int block_size = atoi(argv[2]);
	int inode_count = atoi(argv[3]);
	int datablock_count = atoi(argv[4]);
	char *fs_img = argv[5];
	
	if (strnlen(label, SUPERBLOCK_LABEL_LENGTH-1) < 1 || block_size <= 0 || inode_count <= 0
		|| datablock_count <= 0) {
		usage(argv);
	}
	
	if (block_size % 512) {
		fprintf(stderr, "Block size must be a multiple of 512!\n");
		return EXIT_FAILURE;
	}

	if (inode_count > 65535) {
		fprintf(stderr, "The maximum number of inodes is 65535!\n");
		return EXIT_FAILURE;
	}

	FILE *fs = fopen(fs_img, "w");
	if (!fs) {
		fprintf(stderr, "Failed creating \"%s\"!\n", fs_img);
		return EXIT_FAILURE;
	}

	// superblock
	superblock_t *sb = superblock_create(label, block_size, inode_count, datablock_count);
	superblock_write(sb, fs);

	// inode bitmap
	bitmap_t inode_bitmap = bitmap_create(inode_count);
	bitmap_set(&inode_bitmap, 0, 1);  // inode 0 is reserved by the system (to mark a free inode)
	bitmap_write(&inode_bitmap, sb->inode_bitmap_start, block_size, fs);

	
	// datablock bitmap
	bitmap_t datablock_bitmap = bitmap_create(datablock_count);
	bitmap_set(&datablock_bitmap, 0, 1);  // data block 0 is reserved by the system (to mark a free block)
	bitmap_write(&datablock_bitmap, sb->datablock_bitmap_start, block_size, fs);

	// inodes (empty)
	inode_table_t itab = inode_table_create(inode_count);
	inode_table_write(&itab, sb->inode_start, block_size, fs);
	
	// datablocks (empty)
	uint8_t empty_block[block_size];
	memset(empty_block, 0, block_size);
	for (int i = 0; i < datablock_count; i++) {
		blocks_write(block_size, sb->datablocks_start+i, empty_block, block_size, fs);
	}
	
	fclose(fs);
	
	printf("Image \"%s\" created successfully.\n", fs_img);

	inode_table_destroy(&itab);
	bitmap_destroy(&inode_bitmap);
	bitmap_destroy(&datablock_bitmap);
	superblock_destroy(sb);

	return EXIT_SUCCESS;
}
