#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <assert.h>
#include "superblock.h"
#include "bitmap.h"
#include "blocks.h"
#include "inode.h"
#include "file.h"
#include "fs.h"

/// Copy a block (or less if it's the end of the file contents) from the file contents and add it to the file system.
/// @param sb superblock
/// @param datablock_bitmap bitmap for the data blocks
/// @param data_left number of bytes left till the end of the file
/// @param free_datablocks number of free data blocks left in the file system
/// @param f file to add to the file system
/// @param fs file system for which the file must be added
static int alloc_block_and_copy(superblock_t *sb, bitmap_t *datablock_bitmap, long *data_left, int *free_datablocks, FILE *f, FILE *fs) {
		assert(*data_left >= 0);
		uint bs = sb->block_size;
		int blk = bitmap_reserve_first_available_bit(datablock_bitmap);
		(*free_datablocks)--;
		uint len = ((uint)*data_left >= bs) ? bs : (uint)(*data_left);
		uint8_t buf[len];
		fread(buf, len, 1, f);
		blocks_write(bs, sb->datablocks_start+blk, buf, len, fs); 
		(*data_left) -= len;
		return blk;
}

/// Add a file to a BFS file system image.
/// @param f pointer to the file that must be added to the image
/// @param filename name of the file to be added to the image
/// @param file_size size of the file to be added to the image
/// @param fs file system image for which the file must be added
static bool add_file(FILE *f, char *filename, uint file_size, FILE *fs) {
	superblock_t *sb = superblock_read(fs);
	uint bs = sb->block_size;
	bitmap_t inode_bitmap = bitmap_read(sb->inode_bitmap_start, sb->inode_count, bs, fs);
	bitmap_t datablock_bitmap = bitmap_read(sb->datablock_bitmap_start, sb->datablock_count, bs, fs);
	inode_table_t itab = inode_table_read(sb->inode_count, sb->inode_start, bs, fs);

	if (file_exists(filename, &itab)) {
		fprintf(stderr, "Not adding \"%s\": file already exists!\n", filename);
		return false;
	}

	inode_t *inode = inode_reserve_first_available(&itab, &inode_bitmap);
	if (!inode) {
		fprintf(stderr, "Failed adding \"%s\": no more available inode!\n", filename);
		return false;
	}
	strncpy((char *)inode->name, filename, INODE_FILENAME_LENGTH-1);
	inode->size = file_size;

	long data_left = file_size;
	int free_datablocks = bitmap_get_free_bits(&datablock_bitmap);

	// Setup direct pointers
	for (uint i = 0; i < INODE_DIRECT_PTR_CNT && free_datablocks > 0 && data_left > 0; i++) {
		inode->direct_ptr[i] = alloc_block_and_copy(sb, &datablock_bitmap, &data_left, &free_datablocks, f, fs); 
	}

	// Setup indirect pointers
	uint pointers_per_block = bs/sizeof(inode->direct_ptr[0]);
	
	for (uint i = 0; i < INODE_INDIRECT_PTR_CNT && free_datablocks > 0 && data_left > 0; i++) {
		int datablock = bitmap_reserve_first_available_bit(&datablock_bitmap);
		free_datablocks--;
		inode->indirect_ptr[i] = datablock;
		uint16_t indirect_ptr[pointers_per_block];
		memset(indirect_ptr, 0, bs);  // clear all pointers

		for (uint j = 0; j < pointers_per_block && free_datablocks > 0 && data_left > 0; j++) {
			indirect_ptr[j] = alloc_block_and_copy(sb, &datablock_bitmap, &data_left, &free_datablocks, f, fs); 
		}

		blocks_write(bs, sb->datablocks_start+datablock, indirect_ptr, bs, fs);
	}
	
	if (data_left > 0) {
		fprintf(stderr, "Failed adding \"%s\": not enough data blocks available!\n", filename);
		return false;
	}
	
	bitmap_write(&inode_bitmap, sb->inode_bitmap_start, bs, fs);
	bitmap_write(&datablock_bitmap, sb->datablock_bitmap_start, bs, fs);
	inode_table_write(&itab, sb->inode_start, bs, fs);
	
	bitmap_destroy(&inode_bitmap);
	bitmap_destroy(&datablock_bitmap);
	inode_table_destroy(&itab);
	superblock_destroy(sb);

	return true;
}

/// Command's usage.
/// @param argv command line arguments.
static void usage(char **argv) {
	fprintf(stderr, "Usage: %s file fs_img\n", basename(argv[0]));
	fprintf(stderr, "Add a file to a BFS file system image.\n");
	fprintf(stderr, "file: file to add to the file system image.\n");
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

	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Failed opening \"%s\"!\n", path);
		return EXIT_FAILURE;
	}

	FILE *fs = fopen(fs_img, "r+");
	if (!fs) {
		fprintf(stderr, "Failed opening \"%s\"!\n", fs_img);
		fclose(f);
		return EXIT_FAILURE;
	}

	if (!add_file(f, basename(path), file_size(path), fs))
		return EXIT_FAILURE;

	fclose(f);
	fclose(fs);

	return EXIT_SUCCESS;
}
