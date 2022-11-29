#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "common/types.h"
#include "common/syscall_nb.h"

extern int syscall(syscall_t nb, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

#endif
