#ifndef _FRAME_H_
#define _FRAME_H_

#include "paging.h"
#include "common/mem.h"
#include "common/types.h"
#include "boot/multiboot.h"
#include "boot/module.h"
#include "drivers/term.h"

// Converts a 32-bit address into a frame number
#define ADDR_TO_FRAME_NB(addr) (((uint32_t)addr) >> 12)

// Converts a frame number into a 32-bit address
#define FRAME_NB_TO_ADDR(n) (((uint32_t)n) << 12)

#define FRAME_SIZE 4096

// Returns the number of frames required to store the given number of bytes
#define FRAME_COUNT(size) ((size + FRAME_SIZE - 1) / FRAME_SIZE)

// Initializes the physical frame subsystem, using the specified amount of physical memory.
extern void frame_init(uint_t RAM_in_KB);

// Marks a frame as allocated.
// frame 0 represents physical addresses [0,4095]
// frame 33 represents physical addresses [33*4096,33*4096+4095] = [135168,139263]
extern void frame_mark_used(int frame);

// Marks a frame as free/available.
extern void frame_mark_free(int frame);

// Allocates a frame (4KB) and returns its physical address.
// Returns the physical address of the frame or
// 0xFFFFFFFF if no more frames are available.
//
// REMARKS:
// The physical address is always aligned to a 4KB boundary.
// The frame's content is always zeroed.
extern void *frame_alloc();

// Free a frame.
// REMARK: doesn't check whether the frame was previously allocated or not.
extern void frame_free(void *frame_addr);

// Returns the total number of free frames.
extern uint_t frame_total_free();

#endif