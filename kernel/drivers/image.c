#include "image.h"

void print_image(image_pixel_t *image, const char *size)
{
    int width, height;
    int space_offset = 0;
    int len = strlen(size);

    do
    {
        space_offset++;
    } while ((size[space_offset]) != ' ' && space_offset < len);

    width = (int)atoi(size);
    height = (int)atoi(size + space_offset + 1);

    printf("%d, %d\n", width, height);
    printf("%d\n", width * height);
    printf("sizeof(image_pixel_t) = %d\n", sizeof(image_pixel_t));

    int start_x = get_xcursor() * FONT_WIDTH;
    int start_y = get_ycursor() * FONT_HEIGHT;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int i = x + (width * y);
            image_pixel_t *pixel = image + i;
            set_pixel((uint16_t)start_x + x, (uint16_t)start_y + y, RGB(pixel->r, pixel->g, pixel->b));
        }
    }
}