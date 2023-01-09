// The application MUST be linked to ulibc.o otherwise exit will never be called, hence the sleep(1)

#include "ulibc.h"

void main()
{
    printf("Hello, %s!\n", "world");
    // syscall(SYSCALL_TERM_PUTS, "HELLO.EXE!!!", 0, 0, 0);
    // uint8_t *fb = (uint8_t *)0xFB000000;
    // for (int i = 0; i < 640 * 100; i++)
    // {
    //     fb[i] = 0xff;
    // }
    // term_colors_t cols = { .fg = BLUE, .bg = CYAN };
    // set_colors(cols);
    // printf("=================\n");
    // printf(" HELLO WORLD !!! \n");
    // printf("=================\n");
}
