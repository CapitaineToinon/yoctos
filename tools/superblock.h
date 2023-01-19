#ifndef _SUPERBLOCK_H_
#define _SUPERBLOCK_H_

#include <stdio.h>
#include "common/fs/superblock.h"
#include "common/types.h"

superblock_t *superblock_create(char *label, uint_t block_size, uint_t inode_count, uint_t block_count);
void superblock_destroy(superblock_t *sb);

superblock_t *superblock_read(FILE *fs);
void superblock_write(superblock_t *sb, FILE *fs);

#endif
