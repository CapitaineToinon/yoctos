#include "ulibc.h"

void main()
{
    // TODO

    // Triggers a pagefault exception by writing outside the application's address space.
    int *a = (int *)(0x10000000 - 0x1000);
    printf("%d\n", *a);
}
