#ifndef _FS_H_
#define _FS_H_

#include "common/types.h"
#include "common/file.h"
#include "common/fs/fs.h"

extern void fs_init();

extern int file_stat(const char *filename, stat_t *stat);
extern int file_open(const char *filename);
extern int file_read(int fd, void *buf, uint_t count);
extern int file_seek(int fd, uint_t offset);
extern void file_close(int fd);

extern file_iterator_t file_iterator();
extern bool file_has_next(file_iterator_t *it);
extern void file_next(const char *filename, file_iterator_t *it);

#endif
