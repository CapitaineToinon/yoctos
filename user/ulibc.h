#ifndef _ULIBC_H_
#define _ULIBC_H_

#include "common/types.h"
#include "common/colors.h"
#include "common/string.h"
#include "common/file.h"

extern bool task_exec(char *filename);
extern void exit();  // defined in entrypoint_asm.s

extern void timer_info(uint_t *freq, uint_t *ticks);
extern uint_t get_ticks();
extern void sleep(uint_t ms);

extern void vbe_init(uint_t *width, uint_t *height);
extern void vbe_setpixel(int x, int y, uint16_t color);
extern void vbe_setpixel_syscall(int x, int y, uint16_t color);

extern void read_string(char *buf);
extern int getc();

extern int starts_with(char *prefix, char *str);
extern char *trim(char *line);

extern void putc(char c);
extern void puts(char *str);
extern void printf(char *fmt, ...);
extern void set_colors(term_colors_t cols);

// File functions
extern int file_open(const char *filename);
extern void file_close(int fd);
extern int file_read(int fd, void *buf, uint_t count);
extern int file_seek(int fd, uint_t offset);
extern int file_stat(const char *filename, stat_t *stat);

extern file_iterator_t file_iterator();
extern bool file_has_next(file_iterator_t *it);
extern void file_next(char *filename, file_iterator_t *it);

#endif
