#ifndef _INODE_H_
#define _INODE_H_

#include "common/fs/inode.h"
#include "common/types.h"
#include "bitmap.h"

inode_table_t inode_table_create(uint_t inode_count);
void inode_table_destroy(inode_table_t *itab);

void inode_display(inode_t *inode);

inode_t *inode_get(inode_table_t *itab, uint_t index);

inode_t *inode_reserve_first_available(inode_table_t *itab, bitmap_t *inode_bitmap);

inode_table_t inode_table_read(uint_t inode_count, uint_t inode_start_block, uint_t block_size, FILE *fs);
void inode_table_write(inode_table_t *itab, uint_t inode_start_block, uint_t block_size, FILE *fs);

#endif
