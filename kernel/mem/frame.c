#include "frame.h"
#include "paging.h"
#include "common/mem.h"

// IA-32 can handle an address space of 4GB.
// 4GB = 1048576 pages (4KB each).
// If we represent each frame with 1 bit, we need 1048576 bits = 128KB (131072 bytes).
static uint8_t frame_bitmap[131072];

// Number of frames available (depends on available RAM).
static int page_frame_count;

void frame_mark_used(int frame) {
    // TODO
}

void frame_mark_free(int frame) {
    // TODO
}

static bool is_frame_free(int frame) {
    // TODO
}

void *frame_alloc() {
    // TODO
}

void frame_free(void *frame_addr) {
    // TODO
}

uint_t frame_total_free() {
    // TODO
}

void frame_init(uint_t RAM_in_KB) {
    // TODO
}
