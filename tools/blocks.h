#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "common/types.h"

void blocks_write(uint_t block_size, uint_t first_block, void *data, uint_t data_len, FILE *fs);

void block_read(uint_t block_size, uint_t block, void *dest, FILE *fs);

#endif
