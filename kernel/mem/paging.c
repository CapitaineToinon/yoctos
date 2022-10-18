#include "paging.h"
#include "frame.h"
#include "boot/module.h"
#include "common/mem.h"
#include "drivers/vbe.h"
#include "drivers/term.h"
#include "x86.h"

static PDE_t kernel_pagedir[PAGETABLES_IN_PD] __attribute__((aligned(PAGE_SIZE)));

// Map, in the specified page directory, the page starting at virt_addr into
// the physical address (frame) phys_addr.
// This function dynamically allocates the necessary memory (frames) to store the
// page table (if not previously allocated).
// Return the new allocated page table if one was just allocated, otherwise returns 0.
static PTE_t *mmap_page(PDE_t *pagedir, uint32_t virt_addr, uint32_t phys_addr, enum privilege_t privilege, enum access_t access) {
    // TODO
}

void paging_mmap(PDE_t *pagedir, uint32_t virt_addr, uint32_t phys_addr, uint32_t size, enum privilege_t privilege, enum access_t access) {
    // TODO
}

void paging_init(uint_t RAM_in_KB) {
    // TODO

    paging_load_pagedir((uint32_t)kernel_pagedir);
	paging_enable();

	term_puts("Paging initialized.\n");
}
