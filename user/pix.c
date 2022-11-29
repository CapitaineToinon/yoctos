// The application MUST be linked to ulibc.o otherwise exit will never be called, hence the sleep(1)

#include "ulibc.h"

// For this performance measurment to be meaningful, think of compiling YoctOS
// with "make clean && make run DEBUG=0" which uses compiler optimizations!

void main() {
    // TODO

    // Fill the whole screen many times and measure the time it takes by:
    // - using the syscall implementation of setpixel
    // - using the userspace setpixel implementation (ie. without syscalls)
}
