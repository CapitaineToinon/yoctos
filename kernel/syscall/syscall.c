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

static int syscall_term_puts(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	term_puts((char *)(arg1));
	return 0;
}

static int syscall_term_set_colors(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	term_colors_t *colors = (term_colors_t *)(arg1);
	term_setcolors(*colors);
	return 0;
}

static int syscall_keyb_get_key(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg1);
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	return keyb_get_key();
}

static int syscall_timer_info(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg3);
	UNUSED(arg4);

	uint_t *freq = ((uint_t *)arg1);
	uint_t *ticks = ((uint_t *)arg2);
	*freq = timer_get_freq();
	*ticks = timer_get_ticks();

	return 0;
}

static int syscall_timer_sleep(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	timer_sleep((uint_t)arg1);
	return 0;
}

static int syscall_vbe_fb_info(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);

	vbe_fb_t *output = ((vbe_fb_t *)arg1);
	vbe_fb_t *fb = vbe_get_fb();
	*output = *fb;

	return 0;
}

static int syscall_vbe_setpix(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg4);
	vbe_setpixel((int)arg1, (int)arg2, (uint16_t)arg3);
	return 0;
}

static int syscall_task_exec(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	return task_exec((char *)arg1);
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
int syscall_handler(syscall_t nb, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	term_printf("print from syscall_handler = %d\n", nb);
	if (nb < __SYSCALL_END__)
	{
		return syscall_func[nb](arg1, arg2, arg3, arg4);
	}
	else
	{
		// Invalid syscall
		return -1;
	};
}
