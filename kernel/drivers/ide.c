/**
 * Primary bus IDE read/write routines using PIO mode.
 * This code is CPU intensive and not efficient (otherwise use interrupts and DMA mode).
 * The IDE driver is partially taken from xv6 source code.
 * Exhaustive documentation about ATA PIO mode: https://wiki.osdev.org/ATA_PIO_Mode
 */

#include "common/mem.h"
#include "pmio/pmio.h"
#include "ide.h"
#include "term.h"

// Regular status register on the primary bus
#define STATUS_PORT    0x1F7

// Base port on primary bus
#define DATA_PORT      0x1F0

// Device control register on primary bus
#define CONTROL_PORT   0x3F6

// Abstraction layer to read/write sectors on either a physical ide drive or an in-memory emulated one
typedef struct ide_wrapper_st {
	void (*read_sector)(int sector, void *dst);
	void (*write_sector)(int sector, void *dst);
	uint8_t *emul_base_addr;
} ide_wrapper_t;

static ide_wrapper_t wrapper;

// Private function prototoypes 
static void ide_init_emul(void *first_sect_addr);
static void ide_init_phys();
static void ide_read_sector_emul(int sector, void *dst);
static void ide_write_sector_emul(int sector, void *src);
static void ide_read_sector_phys(int sector, void *dst);
static void ide_write_sector_phys(int sector, void *src);

// =====================================================================
// Public functions

void ide_init(enum ide_mode mode, void *first_sect_addr) {
	if (mode == MODE_EMUL) {
		ide_init_emul(first_sect_addr);
	}
	else if (mode == MODE_HW) {
		ide_init_phys();
	}
	else {
		term_puts("ERROR: ide_init(): invalid mode specified!\n");
		term_puts("Defaulting to hardware mode.\n");
		ide_init_phys();
	}
}

/**
 * Read sectors from the first disk.
 * @param first sector to read (0-indexed)
 * @param dst address to store to read data
 */
void ide_read_sector(int sector, void *dst) {
	wrapper.read_sector(sector, dst);
}

/**
 * Write sectors from the first disk.
 * @param first sector to write (0-indexed)
 * @param src address of the data to be written
 */
void ide_write_sector(int sector, void *src) {
	wrapper.write_sector(sector, src);
}

// =====================================================================
// Private functions

static void ide_init_emul(void *first_sect_addr) {
	wrapper.read_sector = ide_read_sector_emul;
	wrapper.write_sector = ide_write_sector_emul;
	wrapper.emul_base_addr = first_sect_addr;
	
	term_puts("IDE emulation driver initialized.\n");
}

static void ide_init_phys() {
	// Configure the first drive to stop sending interrupts
	uint8_t val = inb(CONTROL_PORT);
	outb(CONTROL_PORT, val | 0x02);

	wrapper.read_sector = ide_read_sector_phys;
	wrapper.write_sector = ide_write_sector_phys;
	wrapper.emul_base_addr = NULL;
	
	term_puts("IDE hardware driver initialized.\n");
}

static void ide_read_sector_emul(int sector, void *dst) {
	memcpydw(dst, wrapper.emul_base_addr+sector*SECTOR_SIZE, SECTOR_SIZE/4);
}

static void ide_write_sector_emul(int sector, void *src) {
	memcpydw(wrapper.emul_base_addr+sector*SECTOR_SIZE, src, SECTOR_SIZE/4);
}

/**
 * Wait for the disk drive to be ready.
 */
static void wait_drive() {
	while ((inb(STATUS_PORT) & 0xC0) != 0x40);
}

/**
 * Prepare the disk drive for read/write at the specified sector in 28-bit LBA mode.
 * @param sector the sector to read or write
 */
static void pio_prepare(int sector) {
	wait_drive();
	outb(0x1F2, 1);  // sector count
	outb(0x1F3, sector & 0xff);  // send bits 0-7 of LBA
	outb(0x1F4, (sector >> 8) & 0xff);  // send bits 8-15 of LBA
	outb(0x1F5, (sector >> 16) & 0xff);  // send bits 16-23 of LBA
	outb(0x1F6, ((sector >> 24) & 0x0f) | 0xe0);  // send bits 24-27 of LBA + set LBA mode; 0xe0 = 11100000b;
}

/**
 * Read sectors from the first disk.
 * @param first sector to read (0-indexed)
 * @param dst address to store to read data
 * Based on the assembly code at http://wiki.osdev.org/ATA_read/write_sectors
 */
static void ide_read_sector_phys(int sector, void *dst) {
	pio_prepare(sector);
	outb(STATUS_PORT, 0x20);  // command port: read with retry
	wait_drive();

	uint16_t *data = (uint16_t *)dst;
	for (int i = 0; i < SECTOR_SIZE/2; i++) {
		*data = inw(DATA_PORT);
		data++;
	}
}

/**
 * Write sectors from the first disk.
 * @param first sector to write (0-indexed)
 * @param src address of the data to be written
 * Based on the assembly code at http://wiki.osdev.org/ATA_read/write_sectors
 */
static void ide_write_sector_phys(int sector, void *src) {
	pio_prepare(sector);
	outb(STATUS_PORT, 0x30);  // command port: write with retry
	wait_drive();

	uint16_t *data = (uint16_t *)src;
	for (int i = 0; i < SECTOR_SIZE/2; i++) {
		outw(DATA_PORT, *data);
		data++;
	}
}
