#ifndef _FILE_COMMON_H_
#define _FILE_COMMON_H_

#include <stdint.h>

typedef struct {
        uint32_t size;
        uint16_t inode;
} stat_t;

typedef struct {
        int val;
} file_iterator_t;

#endif
