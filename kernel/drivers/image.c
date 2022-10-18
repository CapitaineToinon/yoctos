#include "image.h"

void print_image(uint8_t *image, const char *size)
{
    uint_t color_size = 3;
    uint_t width, height;
    uint_t space_offset = 0;
    uint_t len = strlen(size);

    do
    {
        space_offset++;
    } while ((size[space_offset]) != ' ' && space_offset < len);

    width = atoi(size);
    height = atoi(size + space_offset + 1);

    printf("%d, %d\n", width, height);

    int xcursor = get_xcursor();
    int ycursor = get_ycursor();

    for (uint_t y = 0; y < height; y++)
    {
        for (uint_t x = 0; x < width; x++)
        {
            uint_t offset = (uint_t)(x + (y * width)) * color_size;
            uint8_t r = *(image + offset);
            uint8_t g = *(image + offset + 1);
            uint8_t b = *(image + offset + 2);
            set_pixel((uint16_t)x, (uint16_t)y, RGB(r, g, b));
        }
    }
}