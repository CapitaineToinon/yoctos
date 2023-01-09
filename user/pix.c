// The application MUST be linked to ulibc.o otherwise exit will never be called, hence the sleep(1)

#include "ulibc.h"

// For this performance measurment to be meaningful, think of compiling YoctOS
// with "make clean && make run DEBUG=0" which uses compiler optimizations!

#define LOOP 5
#define COLORS_COUNT 5

void main()
{
    // TODO

    uint16_t colors[COLORS_COUNT] = {BLUE, GREEN, RED, CYAN, BLACK};
    uint_t freq = 0;
    uint_t ticks_syscall = 0, ticks = 0;
    uint_t ticks_syscall_after = 0, ticks_after = 0;
    uint_t width = 0, height = 0;

    printf("screen is %d by %d\n", width, height);
    vbe_init(&width, &height);

    // Fill the whole screen many times and measure the time it takes by:
    // - using the syscall implementation of setpixel

    timer_info(&freq, &ticks_syscall);
    for (uint_t loop = 0; loop < LOOP; loop++)
        for (uint_t color = 0; color < COLORS_COUNT; color++)
            for (uint_t y = 0; y < height; y++)
                for (uint_t x = 0; x < width; x++)
                    vbe_setpixel_syscall(x, y, colors[color]);
    timer_info(&freq, &ticks_syscall_after);

    // - using the userspace setpixel implementation (ie. without syscalls)

    timer_info(&freq, &ticks);
    for (uint_t loop = 0; loop < LOOP; loop++)
        for (uint_t color = 0; color < COLORS_COUNT; color++)
            for (uint_t y = 0; y < height; y++)
                for (uint_t x = 0; x < width; x++)
                    vbe_setpixel(x, y, colors[color]);
    timer_info(&freq, &ticks_after);

    printf("screen is %d by %d\n", width, height);

    uint_t syscall_final = ticks_syscall_after - ticks_syscall;
    uint_t final = ticks_after - ticks;

    printf("Number of ticks it took syscalls to draw the screen %d times: %d\n", COLORS_COUNT * LOOP, syscall_final);
    printf("Number of ticks for the same thing accessing fb directly: %d\n", final);
    printf("No syscall is about %d times faster\n", syscall_final / final);
}
