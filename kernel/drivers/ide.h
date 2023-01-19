#ifndef _IDE_H_
#define _IDE_H_

#include "common/types.h"

#define SECTOR_SIZE 512

enum ide_mode {
    MODE_HW,
    MODE_EMUL
};

void ide_init(enum ide_mode mode, void *first_sect_addr);
void ide_read_sector(int sector, void *dst);
void ide_write_sector(int sector, void *src);

#endif
