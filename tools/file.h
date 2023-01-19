#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "inode.h"

long file_size(char *path);

bool file_exists(char *filename, inode_table_t *itab);

#endif
