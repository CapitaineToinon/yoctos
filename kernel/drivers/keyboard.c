#include "common/types.h"
#include "common/keycodes.h"
#include "keymaps/keymap.h"
#include "keyboard.h"
#include "term.h"
#include "pmio/pmio.h"
#include "interrupt/irq.h"

// Keyboard data and status registers
#define DATA_REG 0x60
#define STATUS_REG 0x64

// Key release codes
#define RELEASE_L_SHIFT 0xAA
#define RELEASE_R_SHIFT 0xB6

// Key scancodes
#define SCANCODE_L_SHIFT 0x2A
#define SCANCODE_R_SHIFT 0x36
#define SCANCODE_CTRL 0x1D
#define SCANCODE_ALT 0x38

extern keymap_t *keymap;

#define BUFFER_SIZE 5
uint16_t buffer[BUFFER_SIZE];
uint16_t producer = 0;
uint16_t consumer = 0;

bool shift_left = false;
bool shift_right = false;

static void keyboard_handler()
{
    uint8_t data = inb(DATA_REG);

    if (data == SCANCODE_L_SHIFT)
    {
        shift_left = true;
        return;
    }

    if (data == SCANCODE_R_SHIFT)
    {
        shift_right = true;
        return;
    }

    if (data == RELEASE_L_SHIFT)
    {
        shift_left = false;
        return;
    }

    if (data == RELEASE_R_SHIFT)
    {
        shift_right = false;
        return;
    }

    bool released = data & (1UL << 7);

    if (released)
    {
        return;
    }

    // TODO
    if ((producer + 1) % BUFFER_SIZE == consumer % BUFFER_SIZE)
    {
        // buffer is full I think?
        term_printf("buffer is full");
        return;
    }

    int code = data & ~(1UL << 7);
    int buffer_index = producer++ % BUFFER_SIZE;

    if (shift_left || shift_right)
    {
        buffer[buffer_index] = keymap->shift[code];
    }
    else
    {
        buffer[buffer_index] = keymap->normal[code];
    }
}

void keyb_init()
{
    // TODO
    // Install the keyboard interrupt handler
    handler_t h = {
        .func = &keyboard_handler,
        .name = "keyboard.c",
    };
    irq_install_handler(1, h);

    term_puts("Keyboard initialized.\n");
}

int keyb_get_key()
{
    // TODO
    if (consumer == producer)
    {
        return 0;
    }

    // term_printf("consumer(%d)", consumer);
    return buffer[consumer++ % BUFFER_SIZE];
}
