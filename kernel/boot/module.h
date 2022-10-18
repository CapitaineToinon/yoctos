#ifndef _MODULE_H_
#define _MODULE_H_

#include "multiboot.h"

multiboot_uint32_t get_module_count();
multiboot_module_t *get_module(int i);

#endif