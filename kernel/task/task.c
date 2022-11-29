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
static uint_t task_id = 1;  // incremented whenever a new task is created

// Template page directory for all tasks.
// Since it will never be loaded as a page directory, there is no need to align it to 4KB.
static PDE_t pagedir_templ[PAGETABLES_IN_PD];

// Saves the current page directory into save_pagedir.
static void save_current_pagedir(PDE_t *save_pagedir) {
	PDE_t *current_pagedir = paging_get_current_pagedir();
	for (uint_t i = 0; i < PAGETABLES_IN_PD; i++) {
		save_pagedir[i] = current_pagedir[i];
    }
}

// Creates and returns a task from the fixed pool of tasks.
// This function dynamically allocates the address space of size "size" (in bytes) for the task.
// Returns NULL if it failed.
task_t *task_create(char *name, uint_t addr_space_size) {
    // Tasks GDT entries start at gdt_first_task_entry (each task uses one GDT entry)
	extern gdt_entry_t *gdt_first_task_entry;
    gdt_entry_t *gdt_task_tss = gdt_first_task_entry;

    // TODO

    // Look for a free task and if found:
    // - initializes the task's fields
    // - initializes its GDT entry and TSS selector
    // - initializes its TSS structure
    // - creates its RAM and VBE identity mappings by using the common template page directory
    // - allocates its address space using the "paging_alloc" function

    term_printf("Allocated %dKB of RAM for task %d (\"%s\")\n", alloc_frame_count*PAGE_SIZE/1024, id, name);

    return NULL;
}

// Frees a task previously created with task_create().
// This function frees the task's page frames.
void task_free(task_t *t) {
    // Make sure to free:
    // - every frame allocated for a page
    // - every frame allocated for a page table
    // Note: page.present indicates if a page was mapped

    // TODO

    // Iterates until reaching a NULL pointer indicating that
    // there is no more allocated page table
    for (uint_t pt = 0; t->page_tables[pt]; pt++) {
        // ...
    }

    term_printf("Freed %dKB of RAM (%d page table(s), %d frames)\n",
                (alloc_frame_count)*PAGE_SIZE/1024,
                alloc_pt_count, alloc_frame_count);
}

// Initializes the task subsystem
void tasks_init() {
    memset(tasks, 0, sizeof(tasks));

	// Allocates and initializes the initial TSS: it is where the CPU state
    // will be saved before switching from the kernel to the very first user task.
    extern gdt_entry_t *gdt_initial_tss;
	*gdt_initial_tss = gdt_make_tss(&initial_tss, DPL_KERNEL);
	memset(&initial_tss, 0, sizeof(tss_t));
	initial_tss.ss0 = GDT_KERNEL_DATA_SELECTOR;
	initial_tss.esp0 = ((uint32_t)initial_tss_kernel_stack) + sizeof(initial_tss_kernel_stack);
	initial_tss.cr3 = (uint32_t)paging_get_kernel_pagedir();

    // TODO

	// Creates a common template page directory (pagedir_templ) that will be shared by each task.
    // This mapping:
	// - identity maps the available RAM so the kernel can access it during a syscall as if there
    //   were no paging
    // - identity maps the VBE framebuffer so that tasks can access it without requiring syscalls

	// Loads the task register to point to the initial TSS selector.
	// IMPORTANT: The GDT must already be loaded before loading the task register!
	task_ltr(gdt_entry_to_selector(gdt_initial_tss));

	term_puts("Tasks initialized.\n");
}

// Creates a new task with the content of the specified binary application.
// Once loaded, the task is ready to be executed.
// Returns NULL if it failed (ie. reached max number of tasks).
static task_t *task_load(char *filename) {

    // TODO

    // - Create a new task using the "task_create" function.
    // - Temporarily maps the new task's address space to allow the current task to contiguously
    //   write the binary into the new task's address space (make sure to save the current mapping
    //   before doing so and restoring it once done).
    // - IMPORTANT: beware of pointers! Their content won't be reachable during the temporary
    //   mapping. Thus, make sure to copy their content before hand.

    return new_task;
}

// Loads a task and executes it.
// Returns false if it failed.
bool task_exec(char *filename) {
	task_t *t = task_load(filename);
	if (!t) {
		return false;
    }
    term_colors_t cols = term_getcolors();
    task_switch(t->tss_selector);
    term_setcolors(cols);
    task_free(t);
    return true;
}
