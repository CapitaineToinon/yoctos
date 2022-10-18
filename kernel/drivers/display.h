#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common/colors.h"
#include "common/stdio.h"
#include "boot/multiboot.h"
#include "mem/gdt.h"
#include "x86.h"
#include "drivers/font.h"

#define COLUMNS 80
#define LINES 30
#define PRINTF_BUFFER_MAX 512

int get_xcursor();
int get_ycursor();
void set_cursor(int x, int y);
void set_pixel(uint16_t, uint16_t, uint16_t);
void set_color(int text, int background);
void set_text_color(int color);
void set_background_color(int color);
void cls();
void printf(const char *format, ...);

#endif