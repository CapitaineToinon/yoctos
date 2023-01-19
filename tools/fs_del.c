#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include "fs.h"
#include "superblock.h"
#include "blocks.h"

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s file fs\n", basename(argv[0]));
	fprintf(stderr, "Delete a file from a BFS file system image.\n");
	fprintf(stderr, "file: file to delete.\n");
	fprintf(stderr, "fs: BFS file system image.\n");
	exit(EXIT_FAILURE);
}

/// Delete the specified file from a BFS file system image.
/// @param filename name of the file to delete
/// @param fs the file system image for which the file must be removed
bool delete_file(char *filename, FILE *fs) {
	superblock_t *sb = superblock_read(fs);
	uint bs = sb->block_size;
	bitmap_t inode_bitmap = bitmap_read(sb->inode_bitmap_start, sb->inode_count, bs, fs);
	bitmap_t datablock_bitmap = bitmap_read(sb->datablock_bitmap_start, sb->datablock_count, bs, fs);
	inode_table_t itab = inode_table_read(sb->inode_count, sb->inode_start, bs, fs);

	inode_t *inode = fs_get_file_inode(filename, &itab);
	if (!inode) {
		fprintf(stderr, "File \"%s\" not present in the image!\n", filename);
		return false;
	}

	// Clear bits from inode bitmap
	bitmap_set(&inode_bitmap, inode->number, 0);

	// Clear bits from data block bitmap
	for (uint i = 0; i < INODE_DIRECT_PTR_CNT; i++) {
		uint idx = inode->direct_ptr[i];
		if (idx > 0 && bitmap_get(&datablock_bitmap, idx)) {
			bitmap_set(&datablock_bitmap, idx, 0);
		}
	}

	for (uint i = 0; i < INODE_INDIRECT_PTR_CNT; i++) {
		uint indirect_block = inode->indirect_ptr[i];
		if (indirect_block > 0) {
			if (bitmap_get(&datablock_bitmap, indirect_block)) {
				bitmap_set(&datablock_bitmap, indirect_block, 0);
			}
			uint8_t contents[bs];
			block_read(bs, indirect_block, contents, fs);
			// Go through all pointers in the indirect block
			uint16_t *ptrs = (uint16_t *)contents;
			for (uint j = 0; j < bs/sizeof(inode->direct_ptr[0]); j++) {
				uint idx = ptrs[j];
				if (idx > 0 && bitmap_get(&datablock_bitmap, idx)) {
					bitmap_set(&datablock_bitmap, idx, 0);
				}
			}
		}
	}

	// Clear inode contents
	memset(inode, 0, sizeof(inode_t));

	// Write updated inode bitmap, datablock bitmap and inodes
	bitmap_write(&inode_bitmap, sb->inode_bitmap_start, bs, fs);
	bitmap_write(&datablock_bitmap, sb->datablock_bitmap_start, bs, fs);
	inode_table_write(&itab, sb->inode_start, bs, fs);
	
	bitmap_destroy(&inode_bitmap);
	bitmap_destroy(&datablock_bitmap);
	inode_table_destroy(&itab);
	superblock_destroy(sb);

	return true;
}

/// Application's entry point.
/// @param argc number of command line arguments.
/// @param argv command line arguments.
int main(int argc, char* argv[]) {
	if (argc != 3)
		usage(argv);

	char *filename = argv[1];
	char *fs_img = argv[2];

	FILE *fs = fopen(fs_img, "r+");
	if (!fs) {
		fprintf(stderr, "Failed opening \"%s\"!\n", fs_img);
		return EXIT_FAILURE;
	}

	if (!delete_file(filename, fs))
		return EXIT_FAILURE;

	fclose(fs);

	return EXIT_SUCCESS;
}
