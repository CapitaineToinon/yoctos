#ifndef _X86_H_
#define _X86_H_

// Executes the IRET instruction.
#define iret() asm volatile("iret")

// Disables hardware interrupts.
#define cli() asm volatile("cli")

// Enables hardware interrupts.
#define sti() asm volatile("sti")

// Halt the processor.
// External interrupts wake up the CPU, hence the cli instruction.
static inline void halt() {
	while(1) asm volatile("cli\nhlt");
}

#endif
