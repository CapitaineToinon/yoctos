#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "display.h"
#include "common/string.h"
#include "common/types.h"

struct image_pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
typedef struct image_pixel image_pixel_t;

void print_image(image_pixel_t *image, const char *size);

#endif