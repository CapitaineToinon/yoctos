#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "superblock.h"
#include "alloc.h"
#include "blocks.h"
#include "fs.h"

/// Create a superblock.
/// @param label of the superblock.
/// @param block_size size of a block in bytes.
/// @param inode_count number of inodes in the file system.
/// @param block_count number of data blocks in the file system.
/// @return a pointer to the superblock
superblock_t *superblock_create(char *label, uint_t block_size, uint_t inode_count, uint_t block_count) {
	assert(label);
	assert(block_size > 0);
	assert(block_count > 0);
	assert(inode_count > 0);

	superblock_t *sb = ALLOC(block_size);
	memcpy((char *)sb->signature, FS_SIGNATURE, FS_SIGNATURE_LENGTH-1);
	sb->version_major = VERSION_MAJOR;
	sb->version_minor = VERSION_MINOR;
	strncpy((char *)sb->label, label, SUPERBLOCK_LABEL_LENGTH-1);
	sb->block_size = block_size;

	uint_t current_block = 1;  // Placeholder for various fields (as block indices)
	sb->inode_count = inode_count;
	sb->inode_block_count = ROUND_UP(inode_count*sizeof(inode_t), block_size);
	
	sb->inode_bitmap_start = current_block;
	sb->inode_bitmap_block_count = ROUND_UP(inode_count/8, block_size);
	current_block += sb->inode_bitmap_block_count;

	sb->datablock_count = block_count;
	sb->datablock_bitmap_start = current_block;
	sb->datablock_bitmap_block_count = ROUND_UP(block_count/8, block_size);
	current_block += sb->datablock_bitmap_block_count;
	
	sb->inode_start = current_block;
	
	uint_t inodes_per_block = block_size/sizeof(inode_t);
	uint_t inodes_block_count = ROUND_UP(inode_count, inodes_per_block);
	current_block += inodes_block_count;
	
	sb->datablocks_start = current_block;
	return sb;
}

/// Destroy a superblock.
/// @param sb the superblock to destroy.
void superblock_destroy(superblock_t *sb) {
	assert(sb);
	free(sb);
}

/// Read the file system's superblock and validate it.
/// @param fs the file from which to read the superblock.
/// @return the superblock read.
superblock_t *superblock_read(FILE *fs) {
	assert(fs);
	fseek(fs, 0, SEEK_SET);  // equivalent to rewind(fs)
	uint_t sz = sizeof(superblock_t);
	superblock_t *sb = ALLOC(sz);
	fread(sb, sz, 1, fs);
	// Superblock validation
	if (strncmp((char *)sb->signature, FS_SIGNATURE, strlen(FS_SIGNATURE)) != 0) goto error;
	if (sb->version_major != VERSION_MAJOR) goto error;
	//if (sb->version_minor != VERSION_MINOR) goto error; // minor version numbers should have no impact, thus not tested
	uint_t bs[] = { 512, 1024, 2048, 4096, 8192 };
	bool bs_ok = false;
	for (uint_t i = 0; i < sizeof(bs)/sizeof(bs[0]); i++) {
		if (sb->block_size == bs[i])
			bs_ok = true;
	}
	if (!bs_ok) goto error;
	return sb;
	
error:
	fprintf(stderr, "Invalid superblock!\n");
	exit(EXIT_FAILURE);
}

/// Write a superblock to the file system.
/// @param sb the superblock to write.
/// @param fs the file to which write the superblock.
void superblock_write(superblock_t *sb, FILE *fs) {
	assert(sb);	
	assert(fs);
	blocks_write(sb->block_size, 0, (void *)sb, sb->block_size, fs); 
}
