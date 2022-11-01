#include "common/string.h"
#include "common/stdio.h"
#include "common/mem.h"
#include "boot/module.h"
#include "boot/multiboot.h"
#include "drivers/vbe.h"
#include "drivers/term.h"
#include "mem/gdt.h"
#include "mem/frame.h"
#include "mem/paging.h"
#include "x86.h"

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();
uint_t kernel_start = (uint_t)&ld_kernel_start;
uint_t kernel_end = (uint_t)&ld_kernel_end;

uint16_t *get_fucking_pixel_addr(uint16_t x, uint16_t y)
{
	uint16_t *fb = (uint16_t *)((intptr_t)(3 * 1024 * 1024));
	
	multiboot_info_t *mbi = multiboot_get_info();
	uint16_t pitch_width = (mbi->framebuffer_pitch * 8) / mbi->framebuffer_bpp;
	return fb + x + (pitch_width * (y));
}

void set_my_fucking_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	uint16_t *p = get_fucking_pixel_addr(x, y);
	*p = color;
}

void kernel_main(multiboot_info_t *mbi)
{
	multiboot_set_info(mbi);
	uint_t RAM_in_KB = multiboot_get_RAM_in_KB();

	gdt_init();

	// This function must be initialized first! (before using any term_xxx functions!)
	vbe_init();
	vbe_fb_t *fb = vbe_get_fb();

	term_init();

	paging_init(RAM_in_KB); // must be called AFTER vbe_init()!

	term_printf("YoctOS started\n");
	term_printf("VBE mode %dx%d %dbpp initialized (addr=0x%x, pitch=%d).\n", fb->width, fb->height, fb->bpp, fb->addr, fb->pitch_in_bytes);
	term_printf("Detected %dKB of RAM.\n", RAM_in_KB);
	term_printf("Kernel loaded at [0x%x-0x%x], size=%dKB\n", kernel_start, kernel_end, (kernel_end - kernel_start) / 1024);
	term_printf("FREE RAM starts at %d\n", RAM_in_KB - ((kernel_end - kernel_start) / 1024));
	term_printf("0x%x\n", kernel_end);
	term_printf("0x%x\n", kernel_end);

	modules_display_info();

	// TODO: draw stuff at the framebuffer's new virtual address to validate mapping code works!

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			set_my_fucking_pixel(x, y, RED);
		}
	}

	term_printf("\nSystem halted.");
	halt();
}
