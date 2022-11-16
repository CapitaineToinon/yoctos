#include "common/string.h"
#include "common/stdio.h"
#include "common/mem.h"
#include "common/keycodes.h"
#include "boot/module.h"
#include "boot/multiboot.h"
#include "drivers/vbe.h"
#include "drivers/term.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "interrupt/idt.h"
#include "mem/paging.h"
#include "mem/frame.h"
#include "mem/gdt.h"
#include "x86.h"

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();
uint_t kernel_start = (uint_t)&ld_kernel_start;
uint_t kernel_end = (uint_t)&ld_kernel_end;

void kernel_main(multiboot_info_t *mbi)
{
    multiboot_set_info(mbi);
    uint_t RAM_in_KB = multiboot_get_RAM_in_KB();

    gdt_init();

    // This function must be initialized first! (before using any term_xxx functions!)
    vbe_init();
    vbe_fb_t *fb = vbe_get_fb();

    paging_init(RAM_in_KB); // must be called AFTER vbe_init()!

    term_init();
    term_printf("YoctOS started\n");
    term_printf("VBE mode %dx%d %dbpp initialized (addr=0x%x, pitch=%d).\n", fb->width, fb->height, fb->bpp, fb->addr, fb->pitch_in_bytes);
    term_printf("Detected %dKB of RAM.\n", RAM_in_KB);
    term_printf("%dKB of RAM available.\n", frame_total_free() * FRAME_SIZE / 1024);
    term_printf("Kernel loaded at [0x%x-0x%x], size=%dKB\n", kernel_start, kernel_end, (kernel_end - kernel_start) / 1024);

    modules_display_info();

    // New things
    sti();
    pic_init();
    idt_init();
    timer_init(50);
    keyb_init();

    // Code to test keyboard functions and exceptions support.
    term_setfgcolor(LIGHT_GREEN);
    term_puts("Press F1 to generate a divide by zero exception\n");
    term_puts("Press F4 to generate a general protection fault\n");
    term_puts("Press F5 to generate a page fault\n");
    term_puts("Press F8 to sleep 5 seconds\n");
    term_puts("Press ESC to halt kernel\n");
    term_puts("Press any other key to test the keyboard...\n");
    // term_setfgcolor(LIGHT_RED);
    // term_setbgcolor(BLUE);

    int c;
    uint8_t *p;
    bool quit = false;

    while (!quit)
    {
        // reminder: keyb_get_key returns != 0 if a key was pressed
        if ((c = keyb_get_key()))
        {
            switch (c)
            {
            case KEY_ESC:
                quit = true;
                break;
            case KEY_F1:
                // Executes a division by zero.
                c = 7 / 0;
                break;
            case KEY_F4:
                // Calls a software interrupt that's not handled.
                asm volatile("int $100");
                break;
            case KEY_F5:
                // Writes to a page that's not mapped.
                p = (uint8_t *)(0xFFFFFFFF); // 2^32 - 1 = 4GB - 1
                *p = 42;
                break;
            case KEY_F8:
                term_printf("[sleeping 5 sec...]");
                timer_sleep(5000);
                break;
            default:
                term_putc(c);
            }
        }
    }

    term_printf("\nSystem halted.");
    halt();
}
