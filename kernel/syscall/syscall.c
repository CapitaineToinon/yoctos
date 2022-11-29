#include "boot/multiboot.h"
#include "common/types.h"
#include "mem/gdt.h"
#include "task/task.h"
#include "mem/frame.h"
#include "mem/paging.h"
#include "drivers/term.h"
#include "drivers/vbe.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "syscall.h"
#include "x86.h"

static int syscall_term_puts(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	term_puts((char *)(arg1));
	return 0;
}

// Map syscall numbers to functions
static int (*syscall_func[])(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) = {
	syscall_term_puts,
    syscall_term_set_colors,
	syscall_keyb_get_key,
	syscall_timer_info,
    syscall_timer_sleep,
    syscall_vbe_fb_info,
    syscall_vbe_setpix,
    syscall_task_exec,
};

// Called by the assembly function: _syscall_handler
int syscall_handler(syscall_t nb, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	if (nb < __SYSCALL_END__) {
		return syscall_func[nb](arg1, arg2, arg3, arg4);
	} else {
		// Invalid syscall        
		return -1;
	};
}
