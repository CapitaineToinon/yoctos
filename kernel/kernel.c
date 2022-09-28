#include "common/colors.h"
#include "boot/multiboot.h"
#include "mem/gdt.h"
#include "x86.h"
#include "drivers/font.h"

extern uint8_t font_8x16[CHARS_COUNT * FONT_HEIGHT];

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();
// uint_t kernel_start = (uint_t)&ld_kernel_start;
// uint_t kernel_end = (uint_t)&ld_kernel_end;

#define COLUMNS 80
#define LINES 30
#define CURSOR_WIDTH 8
#define CURSOR_HEIGHT 16

static int xcursor;
static int ycursor;

void cls()
{
    multiboot_info_t *mbi = multiboot_get_info();

    for (multiboot_uint16_t y = 0; y < mbi->framebuffer_height; y++)
    {
        for (multiboot_uint16_t x = 0; x < mbi->framebuffer_width; x++)
        {
            set_pixel(x, y, WHITE);
        }
    }

    xcursor = 0;
    ycursor = 0;
}

multiboot_uint16_t *get_pixel_addr(multiboot_uint16_t x, multiboot_uint16_t y)
{
    multiboot_info_t *mbi = multiboot_get_info();
    multiboot_uint16_t *fb = (multiboot_uint16_t *)mbi->framebuffer_addr;
    multiboot_uint16_t pitch_width = (mbi->framebuffer_pitch * 8) / mbi->framebuffer_bpp;
    return fb + x + (pitch_width * (y));
}

void set_pixel(multiboot_uint16_t x, multiboot_uint16_t y, multiboot_uint16_t color)
{
    multiboot_uint16_t *p = get_pixel_addr(x, y);
    *p = color;
}

multiboot_uint16_t get_pixel(multiboot_uint16_t x, multiboot_uint16_t y)
{
    multiboot_uint16_t *p = get_pixel_addr(x, y);
    return *p;
}

void new_line()
{
    xcursor = 0;
    ycursor++;

    if (ycursor >= LINES)
    {
        multiboot_info_t *mbi = multiboot_get_info();
        for (multiboot_uint16_t y = FONT_HEIGHT; y < mbi->framebuffer_height; y++)
        {
            for (multiboot_uint16_t x = 0; x < mbi->framebuffer_width; x++)
            {
                multiboot_uint16_t color = get_pixel(x, y);
                set_pixel(x, y - FONT_HEIGHT, color);
            }
        }

        for (multiboot_uint16_t y = mbi->framebuffer_height - FONT_HEIGHT; y < mbi->framebuffer_height; y++)
        {
            for (multiboot_uint16_t x = 0; x < mbi->framebuffer_width; x++)
            {
                set_pixel(x, y, WHITE);
            }
        }

        ycursor--;
    }
}

void print_char(char c)
{
    if (c == '\n' || c == '\r')
    {
        new_line();
        return;
    }

    multiboot_uint16_t xoffset = xcursor * FONT_WIDTH;
    multiboot_uint16_t yoffset = ycursor * FONT_HEIGHT;
    uint8_t *character = &font_8x16[c * FONT_HEIGHT];

    for (multiboot_uint16_t y = 0; y < CURSOR_HEIGHT; y++)
    {
        for (multiboot_uint16_t x = 0; x < CURSOR_WIDTH; x++)
        {
            if (*(character + y) & (0b00000001 << x))
            {
                set_pixel(FONT_WIDTH - x + xoffset, y + yoffset, BLACK);
            }
        }
    }

    xcursor++;
    if (xcursor >= COLUMNS)
        new_line();
}

void print_string(char *string, int length)
{
    for (multiboot_uint16_t i = 0; i < length; i++)
    {
        print_char(string[i]);
    }
}

void kernel_main(multiboot_info_t *mbi)
{
    multiboot_set_info(mbi);
    gdt_init();

    cls();

    while (true)
    {
        print_string("bonjourbonjourbonjourbonjourbonjourbonjourbonjour", 7 * 7);
        int i = 0;
        while (true)
        {
            i++;
            if (i > 10000000)
                break;
        }
    }
}
