#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include "superblock.h"
#include "bitmap.h"
#include "file.h"

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s fs\n", basename(argv[0]));
	fprintf(stderr, "List information about a BFS file system image.\n");
	fprintf(stderr, "fs: BFS file system image.\n");
	exit(EXIT_FAILURE);
}

/// Display the superblock information
/// @param sb pointer to a superblock structure in memory
static void display_superblock_info(superblock_t *sb) {
	int8_t signature[5];
	memcpy(signature, sb->signature, 4);
	signature[4] = 0;

	printf("Signature: %s\n", signature);
	printf("Major version: %u\n", sb->version_major);
	printf("Minor version: %u\n", sb->version_minor);
	printf("Label: %s\n", sb->label);
	printf("Block size: %u\n", sb->block_size);
	printf("Inode bitmap start: %u\n", sb->inode_bitmap_start);
	printf("Inode bitmap block count: %u\n", sb->inode_bitmap_block_count);
	printf("Data block bitmap start: %u\n", sb->datablock_bitmap_start);
	printf("Data block bitmap block count: %u\n", sb->datablock_bitmap_block_count);
	printf("Inode start: %u\n", sb->inode_start);
	printf("Inode count: %u\n", sb->inode_count);
	printf("Inode block count: %u\n", sb->inode_block_count);
	printf("Data blocks start: %u\n", sb->datablocks_start);
	printf("Data block count: %u\n", sb->datablock_count);
}

/// Display how much % of the bitmap is currently in use.
/// @param bitmap bitmap for which to compute the usage
/// @param label bitmap "name" used when displaying usage information
static void display_bitmap_usage(bitmap_t *bitmap, char *label) {
	uint total_bits = bitmap->count;
	uint free_bits = bitmap_get_free_bits(bitmap);
	uint used_bits = total_bits-free_bits;
	float percent_used = 100.f*(float)used_bits/(float)total_bits;
	printf("Used %s: %.f%% (%u/%u)\n", label, percent_used, used_bits, total_bits);
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

	superblock_t *sb = superblock_read(fs);
	uint bs = sb->block_size;
	bitmap_t inode_bitmap = bitmap_read(sb->inode_bitmap_start, sb->inode_count, bs, fs);
	bitmap_t datablock_bitmap = bitmap_read(sb->datablock_bitmap_start, sb->datablock_count, bs, fs);
	fclose(fs);

	printf("File name: %s\n", fs_img);
	long size = file_size(fs_img);
	printf("Image size: %ld bytes (%ld KB)\n", size, size/1024);

	display_superblock_info(sb);
	display_bitmap_usage(&inode_bitmap, "inodes");
	display_bitmap_usage(&datablock_bitmap, "data blocks");
	
	superblock_destroy(sb);
	bitmap_destroy(&inode_bitmap);
	bitmap_destroy(&datablock_bitmap);

	return EXIT_SUCCESS;
}

