#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stdio.h>
#include "common/types.h"
#include "common/fs/bitmap.h"

bitmap_t bitmap_create(uint_t bitcount);
void bitmap_destroy(bitmap_t *bitmap);

bitmap_t bitmap_read(uint_t start_block, uint_t bitcount, uint_t block_size, FILE *fs);
void bitmap_write(bitmap_t *bitmap, uint_t start_block, uint_t block_size, FILE *fs);
	
#endif
