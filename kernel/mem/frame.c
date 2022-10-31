#include "frame.h"

// IA-32 can handle an address space of 4GB.
// 4GB = 1_048_576 pages (4KB each).
// If we represent each frame with 1 bit, we need 1_048_576 bits = 128KB (131_072 bytes).
static uint8_t frame_bitmap[131072];

// Number of frames available (depends on available RAM).
static int frame_count;

static bool is_frame_free(int frame)
{
    int index = frame / 8;
    int shift = frame % 8;
    return !(frame_bitmap[index] & (1UL << shift));
}

void frame_mark_used(int frame)
{
    if (!is_frame_free(frame))
    {
        return;
    }

    int index = frame / 8;
    int shift = frame % 8;
    frame_bitmap[index] |= (1UL << shift);
    frame_count--;
}

void frame_mark_free(int frame)
{
    if (is_frame_free(frame))
    {
        return;
    }

    int index = frame / 8;
    int shift = frame % 8;
    frame_bitmap[index] &= ~(1UL << shift);
    frame_count++;
}

void *frame_alloc()
{
    term_printf("frame_alloc\n");

    for (int i = 0; i < 131072 * 8; i++)
    {
        if (is_frame_free(i))
        {
            term_printf("%d is free, giving that\n", i);
            frame_mark_used(i);
            term_printf("frame_total_free %d\n", frame_total_free());
            return (void *)FRAME_NB_TO_ADDR(i);
        }
    }

    term_printf("didn't find a free frame\n");
    return (void *)0xFFFFFF;
}

void frame_free(void *frame_addr)
{
    int nb = ADDR_TO_PAGE_NB(frame_addr);
    term_printf("freeing %d,\n", nb);
    frame_mark_free(nb);
    term_printf("frame_total_free %d\n", frame_total_free());
}

uint_t frame_total_free()
{
    return frame_count;
}

void frame_init(uint_t RAM_in_KB)
{
    // convert to bytes for later
    uint32_t RAM_in_BYTES = (uint32_t)(RAM_in_KB * 1024);
    term_printf("RAM_in_KB %d\n", RAM_in_KB);
    term_printf("RAM_in_BYTES %d\n", RAM_in_BYTES);

    // everything is free by default
    frame_count = 131072 * 8;
    for (int i = 0; i < 131072; i++)
    {
        frame_bitmap[i] = 0;
    }

    term_printf("frame_total_free %d\n", frame_total_free());

    // mark everything outside of the RAM as not available
    uint32_t RAM_frame_size = FRAME_COUNT(RAM_in_BYTES);
    uint32_t RAM_frame_outside = RAM_frame_size + 1;
    uint32_t RAM_frame_outside_end = 131072 * 8;

    term_printf("RAM is can fit in %d frames\n", RAM_frame_size);
    term_printf("frames from %d to %d are outside the RAM reach\n", RAM_frame_outside, RAM_frame_outside_end);
    for (uint32_t i = RAM_frame_outside; i <= RAM_frame_outside_end; i++)
    {
        frame_mark_used(i);
    }

    term_printf("frame_total_free %d\n", frame_total_free());

    uint32_t last_module = modules_last_address();
    uint32_t last_module_frame = ADDR_TO_FRAME_NB(last_module);
    term_printf("frames from 0 to %d are used by kernel + grub + modules\n", last_module_frame);
    for (uint32_t i = 0; i <= last_module_frame; i++)
    {
        frame_mark_used(i);
    }

    term_printf("frame_total_free %d\n", frame_total_free());

    // mark the framebuffer as not available
    multiboot_info_t *mbi = multiboot_get_info();
    uint32_t framebuffer_size = mbi->framebuffer_pitch * mbi->framebuffer_height;
    uint32_t framebuffer_frame = ADDR_TO_FRAME_NB(mbi->framebuffer_addr);
    uint32_t framebuffer_frame_end = ADDR_TO_FRAME_NB(mbi->framebuffer_addr + (multiboot_uint64_t)framebuffer_size);
    term_printf("frames from %d to %d are used by framebuffer\n", framebuffer_frame, framebuffer_frame_end);
    for (uint32_t i = framebuffer_frame; i <= framebuffer_frame_end; i++)
    {
        frame_mark_used(i);
    }

    term_printf("frame_total_free %d\n", frame_total_free());
}
