#include "common/syscall_nb.h"
#include "common/string.h"
#include "common/stdio.h"
#include "common/vbe_fb.h"
#include "ulibc.h"
#include "syscall.h"
#include "ld.h"

#define TAB_SIZE 4

SECTION_DATA static vbe_fb_t fb;

void sleep(uint_t ms)
{
	syscall(SYSCALL_TIMER_SLEEP, (uint32_t)ms, 0, 0, 0);
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
