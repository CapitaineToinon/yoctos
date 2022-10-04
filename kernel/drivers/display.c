#include "display.h"

static int xcursor;
static int ycursor;
static int background_color = BLACK;
static int text_color = WHITE;

// private methods
uint16_t *get_pixel_addr(uint16_t x, uint16_t y);
void set_pixel(uint16_t x, uint16_t y, uint16_t color);
uint16_t get_pixel(uint16_t x, uint16_t y);
void new_line();
void print_char(char c);

void set_color(int text, int background)
{
    set_text_color(text);
    set_background_color(background);
}

void set_text_color(int color)
{
    text_color = color;
}

void set_background_color(int color)
{
    background_color = color;
}

void cls()
{
    multiboot_info_t *mbi = multiboot_get_info();

    for (uint16_t y = 0; y < mbi->framebuffer_height; y++)
    {
        for (uint16_t x = 0; x < mbi->framebuffer_width; x++)
        {
            set_pixel(x, y, background_color);
        }
    }

    xcursor = 0;
    ycursor = 0;
}

void printf(const char *format, ...)
{
    char string[PRINTF_BUFFER_MAX];

    va_list arg_ptr;
    va_start(arg_ptr, format);
    int size = vsnprintf(string, PRINTF_BUFFER_MAX, format, arg_ptr);
    va_end(arg_ptr);

    if (size <= 0)
        return;

    for (uint16_t i = 0; i < PRINTF_BUFFER_MAX; i++)
    {
        if (string[i] == '\0')
            return;

        print_char(string[i]);
    }
}

uint16_t *get_pixel_addr(uint16_t x, uint16_t y)
{
    multiboot_info_t *mbi = multiboot_get_info();
    uint16_t *fb = (uint16_t *)((intptr_t)mbi->framebuffer_addr);
    uint16_t pitch_width = (mbi->framebuffer_pitch * 8) / mbi->framebuffer_bpp;
    return fb + x + (pitch_width * (y));
}

void set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint16_t *p = get_pixel_addr(x, y);
    *p = color;
}

uint16_t get_pixel(uint16_t x, uint16_t y)
{
    uint16_t *p = get_pixel_addr(x, y);
    return *p;
}

void new_line()
{
    xcursor = 0;
    ycursor++;

    if (ycursor >= LINES)
    {
        multiboot_info_t *mbi = multiboot_get_info();
        for (uint16_t y = FONT_HEIGHT; y < mbi->framebuffer_height; y++)
        {
            for (uint16_t x = 0; x < mbi->framebuffer_width; x++)
            {
                uint16_t color = get_pixel(x, y);
                set_pixel(x, y - FONT_HEIGHT, color);
            }
        }

        for (uint16_t y = mbi->framebuffer_height - FONT_HEIGHT; y < mbi->framebuffer_height; y++)
        {
            for (uint16_t x = 0; x < mbi->framebuffer_width; x++)
            {
                set_pixel(x, y, background_color);
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

    uint16_t xoffset = xcursor * FONT_WIDTH;
    uint16_t yoffset = ycursor * FONT_HEIGHT;
    uint8_t *character = &font_8x16[c * FONT_HEIGHT];

    for (uint16_t y = 0; y < FONT_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < FONT_WIDTH; x++)
        {
            int color = (*(character + y) & (1 << x)) ? text_color : background_color;
            set_pixel(FONT_WIDTH - x + xoffset, y + yoffset, color);
        }
    }

    xcursor++;
    if (xcursor >= COLUMNS)
        new_line();
}