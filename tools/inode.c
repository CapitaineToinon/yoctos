#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "inode.h"
#include "bitmap.h"
#include "alloc.h"
#include "blocks.h"

/// Create an inode table.
/// @param inode_count number of inodes to create in the table.
/// @return the newly created inode table.
inode_table_t inode_table_create(uint inode_count) {
	assert(inode_count > 0);
	
	uint sz = inode_count*sizeof(inode_t);
	inode_t *inodes = ALLOC(sz);
	memset(inodes, 0, sz);

	inode_table_t itab = { inode_count, sz, inodes };
	return itab;	
}

/// Destroy an inode table.
/// @param itab the inode table to destroy.
void inode_table_destroy(inode_table_t *itab) {
	assert(itab);
	
	itab->inode_count = 0;
	free(itab->inodes);
}

/// Display the partial contents of an inode to stdout.
/// @param inode the inode to display.
void inode_display(inode_t *inode) {
	assert(inode);
	
	printf("inode={\n");
	printf("  number: %u\n", inode->number);
	printf("  name: %s\n", inode->name);
	printf("  size: %u\n", inode->size);
	for (uint i = 0; i < INODE_DIRECT_PTR_CNT; i++) {
		if (!inode->direct_ptr[i])
			break;
		printf("  direct_ptr[%u]: %u\n", i, inode->direct_ptr[i]);
	}
	for (uint i = 0; i < INODE_INDIRECT_PTR_CNT; i++) {
		if (!inode->indirect_ptr[i])
			break;
		printf("  indirect_ptr[%u]: %u\n", i, inode->indirect_ptr[i]);
	}	
	printf("}\n");
}

/// Read an inode table from a file.
/// @param inode_count the number of inode in the table to read.
/// @param inode_start_block at which block the inode table starts.
/// @param block_size size block size in bytes.
/// @param fs the file stream.
/// @return the inode table read.
inode_table_t inode_table_read(uint inode_count, uint inode_start_block, uint block_size, FILE *fs) {
	assert(inode_count > 0);
	assert(inode_start_block > 0);
	assert(block_size > 0);
	assert(fs);
	
	inode_table_t itab = inode_table_create(inode_count);

	fseek(fs, inode_start_block*block_size, SEEK_SET);
	fread(itab.inodes, inode_count*sizeof(inode_t), 1, fs);
	
	return itab;
}

/// Write an inode table to a file.
/// @param itab the inode table to write.
/// @param inode_start_block at which block the inode table should be written.
/// @param block_size size block size in bytes.
/// @param fs the file stream.
void inode_table_write(inode_table_t *itab, uint inode_start_block, uint block_size, FILE *fs) {
	blocks_write(block_size, inode_start_block, itab->inodes, itab->size, fs);
}
