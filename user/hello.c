// The application MUST be linked to ulibc.o otherwise exit will never be called, hence the sleep(1)

#include "ulibc.h"

void main() {
    term_colors_t cols = { .fg = BLUE, .bg = CYAN };
    set_colors(cols);
    printf("=================\n");
    printf(" HELLO WORLD !!! \n");
    printf("=================\n");
}
