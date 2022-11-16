#include "common/types.h"
#include "timer.h"
#include "term.h"
#include "pmio/pmio.h"
#include "interrupt/irq.h"
#include "logo.h"
#include "mem/paging.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define MIN_FREQ 18
#define MAX_FREQ 1193182

// We want to make sure ticks is never cached when being read.
// Also we don't want the compiler to optimize its access with -O3.
static volatile uint_t ticks = 0; // Current ticks count

static uint_t freq;

// Timer interrupt handler
// Update ticks count and display a logo (animated)
static void timer_handler()
{
    ticks++;
    logo_render();
}

// By default, the PIT generates an IRQ0 18.2065x per sec.
void timer_init(uint_t freq_hz)
{
    // TODO
    // Setup timer at freq_hz
    freq = freq_hz <= (uint_t)MIN_FREQ ? MIN_FREQ : freq_hz;
    uint_t divider = freq_hz < (uint_t)MIN_FREQ ? 0 : MAX_FREQ / freq_hz;

    outb(PIT_COMMAND, 36);
    outb(PIT_CHANNEL0, (uint8_t)divider);
    outb(PIT_CHANNEL0, (uint8_t)(divider >> 8));

    // TODO
    // Install the timer interrupt handler (function timer_handler() above)
    handler_t h = {
        .func = &timer_handler,
        .name = "timer.c",
    };
    irq_install_handler(0, h);

    term_printf("Timer initialized (%dHz).\n", freq);
    logo_init();
}

void timer_sleep(uint_t ms)
{
    uint_t needed = ticks + (ms * freq / 1000);
    while (ticks < needed)
        ;
}

uint_t timer_get_freq()
{
    return freq;
}

uint_t timer_get_ticks()
{
    return ticks;
}
