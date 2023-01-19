#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include "blocks.h"

/// Write data_len bytes of data starting at the specified block (0-indexed).
/// If data size is not a multiple of block size, the end of the block is padded with zeroes.
/// @param block_size size of blocks in bytes
/// @param first_block block where to start writing the data
/// @param data data to write
/// @param data_len length (in bytes) of the data to write
/// @param fs file system to write the block(s) to
void blocks_write(uint block_size, uint first_block, void *data, uint data_len, FILE *fs) {
	assert(block_size > 0);
	assert(data);
	assert(data_len > 0);
	assert(fs);
	
	uint offset = block_size*first_block;
	fseek(fs, offset, SEEK_SET);
	fwrite(data, data_len, 1, fs);
	
	uint padding_len = (data_len < block_size ? block_size-data_len : data_len % block_size);

	if (padding_len) {
		uint8_t padding[padding_len];
		memset(padding, 0, padding_len);
		fwrite(padding, padding_len, 1, fs);
	}
}

/// Read the specified block and stores its contents in dest.
/// @param block_size size of blocks in bytes
/// @param block block number to read (0-indexed).
/// @param dest where to write the block read.
/// @param fs the file system to read the block from.
void block_read(uint block_size, uint block, void *dest, FILE *fs) {
	assert(block_size > 0);
	assert(block > 0);
	assert(dest);
	assert(fs);
	
	uint offset = block_size*block;
	fseek(fs, offset, SEEK_SET);
	fread(dest, block_size, 1, fs);
}
