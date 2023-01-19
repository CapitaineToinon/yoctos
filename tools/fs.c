#include <string.h>
#include <assert.h>
#include "fs.h"
#include "blocks.h"
#include "inode.h"
#include "bitmap.h"

/// Retrieve the inode for a given file name.
/// @param filename the name of the file.
/// @param itab inode table.
/// @return the inode matching the file name or NULL if no such file exists in the file system.
inode_t *fs_get_file_inode(char *filename, inode_table_t *itab) {
	assert(filename);
	assert(itab);
	
	for (uint_t i = 0; i < itab->inode_count; i++) {
		inode_t *inode = inode_get(itab, i);
		if (inode->number > 0) {  // inode numbers > 0 are in use
			if (strncmp(filename, (char *)inode->name, INODE_FILENAME_LENGTH-1) == 0)
				return inode;
		}
	}
	return NULL;
}

/// Return the physical block corresponding to a given offset (in bytes) in a file.
/// @param offset the offset in the file in bytes.
/// @param inode the inode of the file.
/// @param sb the superblock of the file system.
/// @param fs the file system.
/// @return the offset or -1 if the offset is out of range.
int fs_offset_to_block(uint_t offset, inode_t *inode, superblock_t *sb, FILE *fs) {
	assert(inode);
	assert(sb);
	assert(fs);
	
	uint_t bs = sb->block_size;
	
	if (offset < INODE_DIRECT_PTR_CNT*bs)
		return inode->direct_ptr[offset/bs];

	uint_t pointers_per_block = bs/sizeof(inode->direct_ptr[0]);

	// Offset out of range?
	if (offset > INODE_DIRECT_PTR_CNT*bs+INODE_INDIRECT_PTR_CNT*pointers_per_block*bs)
		return -1;

	offset -= INODE_DIRECT_PTR_CNT*bs;
	uint_t indirect_block_idx = offset/(pointers_per_block*bs);
	uint_t indirect_block = inode->indirect_ptr[indirect_block_idx];
	
	uint16_t buf[pointers_per_block];
	block_read(bs, sb->datablocks_start+indirect_block, (void *)buf, fs);
	offset -= indirect_block_idx*(pointers_per_block*bs);
	return buf[offset/bs];
}

/// Read a portion of a file and write it to a destination buffer.
/// @param filename the name of the file.
/// @param offset the offset at which to start reading the file.
/// @param count how many bytes to read.
/// @param dest where to write the bytes read (MUST be allocated by the caller!).
/// @param fs the file system.
/// @return true if it succeeded or false otherwise.
bool fs_read(char *filename, uint_t offset, uint_t count, void *dest, FILE *fs) {
	assert(filename);
	assert(dest);
	assert(fs);
	
	superblock_t *sb = superblock_read(fs);
	uint_t bs = sb->block_size;
	bitmap_t inode_bitmap = bitmap_read(sb->inode_bitmap_start, sb->inode_count, bs, fs);
	bitmap_t datablock_bitmap = bitmap_read(sb->datablock_bitmap_start, sb->datablock_count, bs, fs);
	inode_table_t itab = inode_table_read(sb->inode_count, sb->inode_start, bs, fs);

	inode_t *inode = fs_get_file_inode(filename, &itab);
	if (!inode) {
		fprintf(stderr, "File not found (\"%s\")!\n", filename);
		return false;
	}

	uint8_t buf[bs];

	int bytes_left = count;
	uint_t dest_offset = 0;
	while (bytes_left > 0) {
		int block = fs_offset_to_block(offset, inode, sb, fs);
		block_read(bs, sb->datablocks_start+block, buf, fs);
		uint_t start_offset = offset % bs;
		uint_t partial_count = bs-start_offset;
		uint_t n = (partial_count < (uint_t)bytes_left) ? partial_count : (uint_t)bytes_left;
		memcpy((uint8_t *)dest+dest_offset, buf+start_offset, n);
		bytes_left -= n;
		offset += n;
		dest_offset += n;
	}
	
	bitmap_destroy(&inode_bitmap);
	bitmap_destroy(&datablock_bitmap);
	inode_table_destroy(&itab);
	superblock_destroy(sb);
	return true;
}
