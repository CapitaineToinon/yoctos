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
static PTE_t *mmap_page(PDE_t *pagedir, uint32_t virt_addr, uint32_t phys_addr, enum privilege_t privilege, enum access_t access)
{
    uint32_t pde = ADDR_TO_PDE(virt_addr);

    if (pagedir[pde].present == false)
    {
        PTE_t *page_table = frame_alloc();
        pagedir[pde].pagetable_base_addr = ADDR_TO_PAGE_NB(page_table);
        pagedir[pde].user = privilege;
        pagedir[pde].rw = access;
        pagedir[pde].present = true;
    }

    PTE_t *page_table = (PTE_t *)PAGE_NB_TO_ADDR(pagedir[pde].pagetable_base_addr);
    uint32_t page = ADDR_TO_PAGE_NB(virt_addr) & 1023; // = % 1024

    page_table[page].frame_base_addr = ADDR_TO_FRAME_NB(phys_addr);
    page_table[page].user = privilege;
    page_table[page].rw = access;
    page_table[page].present = true;

    return page_table;
}

void paging_mmap(PDE_t *pagedir, uint32_t virt_addr, uint32_t phys_addr, uint32_t size, enum privilege_t privilege, enum access_t access)
{
    uint32_t page_count = PAGE_COUNT(size);

    for (uint32_t i = 0; i < page_count; i++)
    {
        uint32_t from = virt_addr + PAGE_NB_TO_ADDR(i);
        uint32_t to = phys_addr + PAGE_NB_TO_ADDR(i);
        mmap_page(pagedir, from, to, privilege, access);
    }
}

void paging_init(uint_t RAM_in_KB)
{
    uint_t RAM_in_BYTES = RAM_in_KB * 1024;
    frame_init(RAM_in_KB);

    multiboot_info_t *mbi = multiboot_get_info();
    uint32_t kernel_addr = 0;
    paging_mmap(kernel_pagedir, kernel_addr, kernel_addr, RAM_in_BYTES, PRIVILEGE_KERNEL, ACCESS_READWRITE);

    uint32_t framebuffer_phys_addr = (uint32_t)mbi->framebuffer_addr;
    uint32_t framebuffer_size = mbi->framebuffer_pitch * mbi->framebuffer_height;

    paging_mmap(kernel_pagedir, framebuffer_phys_addr, framebuffer_phys_addr, framebuffer_size, PRIVILEGE_KERNEL, ACCESS_READWRITE);

    uint32_t framebuffer_virt_addr = 3 * 1024 * 1024;
    paging_mmap(kernel_pagedir, framebuffer_virt_addr, framebuffer_phys_addr, framebuffer_size, PRIVILEGE_KERNEL, ACCESS_READWRITE);

    paging_load_pagedir((uint32_t)kernel_pagedir);
    paging_enable();

    term_puts("Paging initialized.\n");
}
