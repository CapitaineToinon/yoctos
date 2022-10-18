#include "module.h"

multiboot_uint32_t get_module_count()
{
    multiboot_info_t *mbi = multiboot_get_info();
    return mbi->mods_count;
}

multiboot_module_t *get_module(int i)
{
    multiboot_info_t *mbi = multiboot_get_info();
    return mbi->mods_addr + (sizeof(multiboot_module_t) * i);
}

// get for module i...