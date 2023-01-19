#ifndef _FS_H_
#define _FS_H_

#include "common/fs/fs.h"
#include "common/types.h"
#include "inode.h"
#include "superblock.h"

/// On disk structure of the file system:
/// Superblock   (starts at block 0, length: 1 block)
/// Inode bitmap (starts at block inode_bitmap_start, length: inode_bitmap_block_count blocks)
/// Block bitmap (starts at block datablock_bitmap_start, length: datablock_bitmap_block_count blocks)
/// Inode table  (starts at block inode_start, length: inode_block_count blocks)
/// Data blocks  (starts at block datablocks_start, length: datablock_count blocks)

inode_t *fs_get_file_inode(char *filename, inode_table_t *itab);

int fs_offset_to_block(uint_t offset, inode_t *inode, superblock_t *sb, FILE *fs);

bool fs_read(char *file_name, uint_t offset, uint_t count, void *dest, FILE *fs);
	
#endif
