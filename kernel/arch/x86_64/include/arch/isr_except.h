#ifndef ARCH_ISR_EXCEPT_H
#define ARCH_ISR_EXCEPT_H

#include <stdint.h>

struct isr_except_spec {
	uintptr_t addr;
	uint8_t gate;
};

struct isr_except_frame_no_code {
	uint64_t rip, cs, flags, rsp, ss;
} __attribute__((packed));

struct isr_except_frame_code {
	uint64_t e, rip, cs, flags, rsp, ss;
} __attribute__((packed));

// when loading the IDT, they are taken from this table.
// see `idt.c` for details.
extern struct isr_except_spec const isr_except_spec_tab[32];

#endif
