#include "common/syscall_nb.h"
#include "common/string.h"
#include "common/stdio.h"
#include "common/vbe_fb.h"
#include "ulibc.h"
#include "syscall.h"
#include "ld.h"

#define TAB_SIZE 4

SECTION_DATA static vbe_fb_t fb;

void vbe_init(uint_t *width, uint_t *height)
{
	syscall(SYSCALL_VBE_FB_INFO, (uint32_t)&fb, 0, 0, 0);
	*width = fb.width;
	*height = fb.height;
}

void vbe_setpixel(int x, int y, uint16_t color)
{
	uint16_t *pixel = fb.addr + x + (fb.pitch_in_pix * y);
	*pixel = color;
}

void vbe_setpixel_syscall(int x, int y, uint16_t color)
{
	syscall(SYSCALL_VBE_SETPIX, (uint32_t)x, (uint32_t)y, (uint32_t)color, 0);
}

void sleep(uint_t ms)
{
	syscall(SYSCALL_TIMER_SLEEP, (uint32_t)ms, 0, 0, 0);
}

int getc()
{
	return syscall(SYSCALL_KEYB_GET_KEY, 0, 0, 0, 0);
}

void putc(char c)
{
	char str1[2] = {c, '\0'};
	puts(str1);
}

void puts(char *s)
{
	syscall(SYSCALL_TERM_PUTS, (uint32_t)s, 0, 0, 0);
}

bool task_exec(char *filename)
{
	return syscall(SYSCALL_TASK_EXEC, (uint32_t)filename, 0, 0, 0);
}

void printf(char *fmt, ...)
{
	char string[512];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	int size = vsnprintf(string, 512, fmt, arg_ptr);
	va_end(arg_ptr);

	if (size <= 0)
		return;

	puts(string);
}

void timer_info(uint_t *freq, uint_t *ticks)
{
	syscall(SYSCALL_TIMER_INFO, (uint32_t)freq, (uint32_t)ticks, 0, 0);
}

void read_string(char *buf)
{
	char *start = buf;
	for (;;)
	{
		int c = getc();
		if (c)
		{
			// backspace
			if (c == '\b')
			{
				if (buf - start > 0)
				{
					putc(c);
					buf--;
				}
			}
			// return
			else if (c == '\n')
			{
				break;
			}
			// tab
			else if (c == '\t')
			{
				c = ' ';
				for (int i = 0; i < TAB_SIZE; i++)
				{
					putc(c);
					*buf = c;
					buf++;
				}
			}
			else
			{
				putc(c);
				*buf = c;
				buf++;
			}
		}
	}
	*buf = 0;
}

// Return 1 if string str starts with prefix.
// Return 0 otherwise.
int starts_with(char *prefix, char *str)
{
	while (*prefix)
	{
		if (*str != *prefix)
		{
			return 0;
		}
		prefix++;
		str++;
	}
	return 1;
}

char *trim(char *line)
{
	int len;

	// Remove heading spaces.
	while (*line == ' ')
		line++;

	// Remove trailing spaces.
	len = strlen(line);
	if (len > 0)
	{
		char *s = line + len - 1;
		int cut = 0;
		while (*s == ' ')
		{
			s--;
			cut = 1;
		}
		if (cut)
		{
			*(s + 1) = 0;
		}
	}

	return line;
}
