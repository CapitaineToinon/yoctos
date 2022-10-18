#include "drivers/display.h"
#include "drivers/image.h"
#include "boot/module.h"
#include "mem/gdt.h"

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();

static intptr_t kernel_start = (intptr_t)&ld_kernel_start;
static intptr_t kernel_end = (intptr_t)&ld_kernel_end;

void kernel_main(multiboot_info_t *mbi)
{
	multiboot_set_info(mbi);
	gdt_init();

	printf("YoctOS started\n");
	printf("VBE mode %dx%d %dbpp intitialized (addr=0x%x, pitch=%d)\n",
		   mbi->framebuffer_width,
		   mbi->framebuffer_height,
		   mbi->framebuffer_bpp,
		   mbi->framebuffer_addr,
		   mbi->framebuffer_pitch);

	printf("Detected %d of RAM\n",
		   multiboot_get_RAM_in_KB());

	printf("Kernel loaded at [0x%x-0x%x]\n",
		   kernel_start,
		   kernel_end);

	multiboot_uint32_t count = get_module_count();
	printf("%d module(s) found\n", count);

	set_text_color(GREEN);
	printf("---------------------\n");
	printf("Welcome to YoctOS !!!\n");
	printf("---------------------\n\n");
	set_text_color(WHITE);

	printf("Color tests:\n");
	set_color(YELLOW, RED);
	printf("Yellow text on red background\n");
	set_color(BLACK, BLUE);
	printf("Black text on blue background\n");

	set_color(WHITE, BLACK);
	for (multiboot_uint32_t i = 0; i < count; i++)
	{
		multiboot_module_t *module = get_module(i);
		printf("Module %d addr=0x%x, size=%d cmdline=\"%s\"\n", i, module->mod_start, module->mod_end - module->mod_start, module->cmdline);

		uint8_t *image = (uint8_t *)module->mod_start;
		print_image(image, (const char *)module->cmdline);
	}
	halt();
}
