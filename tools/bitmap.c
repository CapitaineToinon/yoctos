#include <stdlib.h>
#include <assert.h>
#include "bitmap.h"
#include "alloc.h"
#include "blocks.h"
#include "fs.h"

#define ASSERT_BLOCK_SIZE assert(block_size == 512 || block_size == 1024 || block_size == 2048 || block_size == 4096 || block_size == 8192)

/// Create a bitmap.
/// @param bitcount number of bits in the bitmap.
/// @return the created bitmap.
bitmap_t bitmap_create(uint_t bitcount) {
	assert(bitcount > 0);
	uint_t bytes = ROUND_UP(bitcount, 8);
	bitmap_t bitmap;
	bitmap.count = bitcount;
	bitmap.size = bytes;
	bitmap.bits = ALLOC(bytes);
	return bitmap;
}

/// Destroy a bitmap.
/// @param bitmap the bitmap to destroy.
void bitmap_destroy(bitmap_t *bitmap) {
	assert(bitmap);
	free(bitmap->bits);
}

/// Read a bitmap from a file system.
/// @param start_block block at which the bitmap starts.
/// @param bitcount number of bits in the bitmap.
/// @param block_size the block size in bytes.
/// @param fs the file system to read the bitmap from.
/// @return the bitmap read from the file system.
bitmap_t bitmap_read(uint_t start_block, uint_t bitcount, uint_t block_size, FILE *fs) {
	ASSERT_BLOCK_SIZE;
	assert(fs);
	bitmap_t bitmap = bitmap_create(bitcount);	
	fseek(fs, start_block*block_size, SEEK_SET);
	fread(bitmap.bits, bitmap.size, 1, fs);
	return bitmap;
}

/// Write a bitmap to a file system.
/// @param bitmap.
/// @param start_block block at which the bitmap starts.
/// @param block_size the block size in bytes.
/// @param fs the file system to read the bitmap from.
void bitmap_write(bitmap_t *bitmap, uint_t start_block, uint_t block_size, FILE *fs) {
	ASSERT_BLOCK_SIZE;
	assert(bitmap);
	assert(fs);
	blocks_write(block_size, start_block, bitmap->bits, bitmap->size, fs);
}
