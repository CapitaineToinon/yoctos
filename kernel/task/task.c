#include "boot/multiboot.h"
#include "boot/module.h"
#include "common/mem.h"
#include "common/string.h"
#include "descriptors.h"
#include "mem/gdt.h"
#include "mem/frame.h"
#include "drivers/term.h"
#include "drivers/vbe.h"
#include "task.h"
#include "x86.h"
#include "tss.h"

#define TASK_STACK_SIZE_MB 2

static task_t tasks[MAX_TASK_COUNT];
static tss_t initial_tss;
static uint8_t initial_tss_kernel_stack[65536];
static uint_t task_id = 0; // incremented whenever a new task is created

// Template page directory for all tasks.
// Since it will never be loaded as a page directory, there is no need to align it to 4KB.
static PDE_t pagedir_templ[PAGETABLES_IN_PD];

// returns the first free task available
static task_t *next_free_task()
{
    for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)
    {
        if (tasks[i].in_use == false)
        {
            term_printf("found space as %d\n", i);
            return &tasks[i];
        }
    }

    return NULL;
}

// Creates and returns a task from the fixed pool of tasks.
// This function dynamically allocates the address space of size "size" (in bytes) for the task.
// Returns NULL if it failed.
static task_t *task_create(char *name, uint_t addr_space_size)
{

    // TODO

    task_t *t = next_free_task();
    if (t == NULL)
    {
        // not space for a new task
        term_printf("no space found\n");
        return NULL;
    }

    memset(t, 0, sizeof(t));

    // Look for a free task and if found:
    // - initializes the task's fields
    t->id = task_id++;
    t->in_use = true;

    t->addr_space_size = addr_space_size;
    uint_t full_task_space_size = t->addr_space_size + (TASK_STACK_SIZE_MB * 1024 * 1024);

    // Tasks GDT entries start at gdt_first_task_entry (each task uses one GDT entry)
    extern gdt_entry_t *gdt_first_task_entry;
    gdt_entry_t *gdt_task_tss = gdt_first_task_entry + t->id;
    // - initializes its GDT entry and TSS selector
    // - initializes its TSS structure
    int gdt_tss_sel = gdt_entry_to_selector(gdt_task_tss);
    *gdt_task_tss = gdt_make_tss(&t->tss, DPL_KERNEL);
    t->tss_selector = gdt_tss_sel;
    term_printf("t->tss_selector = %d\n", t->tss_selector);

    // setup initial task context
    tss_t *tss = &t->tss;
    t->virt_addr = TASK_VIRT_ADDR;
    tss->cs = GDT_USER_CODE_SELECTOR;
    tss->ds = tss->es = tss->fs = tss->gs = tss->ss = GDT_USER_DATA_SELECTOR;
    tss->cr3 = (uint32_t)t->pagedir; // task's page directory
    tss->eflags = (1UL << 9);
    tss->ss0 = GDT_KERNEL_DATA_SELECTOR;
    tss->esp0 = (uint32_t)t->kernel_stack + sizeof(t->kernel_stack);
    tss->eip = TASK_VIRT_ADDR;
    tss->esp = tss->ebp = TASK_VIRT_ADDR + full_task_space_size;

    // - creates its RAM and VBE identity mappings by using the common template page directory
    memcpy(t->pagedir, pagedir_templ, sizeof(pagedir_templ));

    // - allocates its address space using the "paging_alloc" function
    uint_t alloc_frame_count = paging_alloc(t->pagedir, t->page_tables, TASK_VIRT_ADDR, full_task_space_size, PRIVILEGE_USER);
    term_printf("Allocated %dKB of RAM (%d frames) for task %d (\"%s\")\n",
                alloc_frame_count * PAGE_SIZE / 1024,
                alloc_frame_count,
                t->id, name);
    return t;
}

// Frees a task previously created with task_create().
// This function frees the task's page frames.
static void task_free(task_t *t)
{
    // Make sure to free:
    // - every frame allocated for a page
    // - every frame allocated for a page table
    // Note: page.present indicates if a page was mapped

    // TODO

    // Iterates until reaching a NULL pointer indicating that
    // there is no more allocated page table
    uint32_t alloc_frame_count = 0;
    uint32_t alloc_pt_count = 0;

    for (uint_t i = 0; t->page_tables[i]; i++)
    {
        PTE_t *page_table = t->page_tables[i];

        for (int j = 0; j < PAGES_IN_PT; j++)
        {
            PTE_t page_entry = page_table[j];

            if (!page_entry.present)
            {
                continue;
            }

            frame_free((void *)FRAME_NB_TO_ADDR(page_entry.frame_base_addr));
            alloc_frame_count++;
        }

        alloc_pt_count++;
    }

    t->in_use = false;
    task_id--;

    term_printf("Freed %dKB of RAM (%d page table(s), %d frames)\n",
                (alloc_frame_count)*PAGE_SIZE / 1024,
                alloc_pt_count, alloc_frame_count);
}

// Initializes the task subsystem
void tasks_init()
{
    memset(tasks, 0, sizeof(tasks));

    // Allocates and initializes the initial TSS: it is where the CPU state
    // will be saved before switching from the kernel to the very first user task.
    extern gdt_entry_t *gdt_initial_tss;
    *gdt_initial_tss = gdt_make_tss(&initial_tss, DPL_KERNEL);
    memset(&initial_tss, 0, sizeof(tss_t));
    initial_tss.ss0 = GDT_KERNEL_DATA_SELECTOR;
    initial_tss.esp0 = ((uint32_t)initial_tss_kernel_stack) + sizeof(initial_tss_kernel_stack);
    initial_tss.cr3 = (uint32_t)paging_get_current_pagedir();

    // TODO
    // Creates a common template page directory (pagedir_templ) that will be shared by each task.
    // This mapping:
    // - identity maps the available RAM so the kernel can access it during a syscall as if there
    //   were no paging
    paging_mmap(pagedir_templ, 0, 0, multiboot_get_RAM_in_KB() * 1024, PRIVILEGE_KERNEL, ACCESS_READWRITE);
    // - identity maps the VBE framebuffer so that tasks can access it without requiring syscalls
    vbe_fb_t *fb = vbe_get_fb();
    paging_mmap(pagedir_templ, (uint32_t)fb->addr, (uint32_t)fb->addr, fb->size, PRIVILEGE_USER, ACCESS_READWRITE);

    // Loads the task register to point to the initial TSS selector.
    // IMPORTANT: The GDT must already be loaded before loading the task register!
    task_ltr(gdt_entry_to_selector(gdt_initial_tss));

    term_puts("Tasks initialized.\n");
}

// Creates a new task with the content of the specified binary application.
// Once loaded, the task is ready to be executed.
// Returns NULL if it failed (ie. reached max number of tasks).
static task_t *task_load(char *filename)
{

    // TODO

    // - Create a new task using the "task_create" function.
    void *mod_start = module_addr_by_name(filename);
    if (mod_start == NULL)
    {
        term_printf("failed to find module\n");
        return NULL;
    }

    uint_t mod_size = module_size_by_name(filename);
    term_printf("mod_size of %d\n", mod_size);
    task_t *new_task = task_create(filename, mod_size);

    if (new_task == NULL)
    {
        term_printf("failed to create task\n");
        return NULL;
    }

    // - Temporarily maps the new task's address space (page directory) to allow the current task to
    // contiguously write the binary into the new task's address space (make sure to save the current
    // mapping (page directory) before doing so and restoring it once done).
    // - IMPORTANT: beware of pointers pointing to addresses in the current task!
    // Their content won't be reachable during the temporary mapping of the new task.
    // Thus, make sure to either reference them before the mapping (or copy their content before hand).
    PDE_t *current = paging_get_current_pagedir();
    paging_load_pagedir(new_task->pagedir);
    memcpy((void *)new_task->virt_addr, (void *)mod_start, mod_size);
    paging_load_pagedir(current);

    return new_task;
}

// Loads a task and executes it.
// Returns false if it failed.
bool task_exec(char *filename)
{
    task_t *t = task_load(filename);

    if (!t)
    {
        term_printf("failed to load\n");
        return false;
    }

    term_colors_t cols = term_getcolors();
    term_printf("before switch\n");
    task_switch(t->tss_selector);
    term_printf("after switch\n");
    term_setcolors(cols);

    task_free(t);
    return true;
}
