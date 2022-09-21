#include "boot/multiboot.h"
#include "mem/gdt.h"
#include "x86.h"

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();
uint_t kernel_start = (uint_t)&ld_kernel_start;
uint_t kernel_end = (uint_t)&ld_kernel_end;

void kernel_main(multiboot_info_t *mbi) {
    multiboot_set_info(mbi);

    gdt_init();
}
