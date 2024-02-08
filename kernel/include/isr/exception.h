#ifndef ISR_EXCEPTION_H__
#define ISR_EXCEPTION_H__

#include <stdint.h>

struct ex_spec {
	uintptr_t addr;
	uint8_t gate;
};

struct ex_frame_no_code {
	uint64_t rip, cs, flags, rsp, ss;
} __attribute__((packed));

struct ex_frame_code {
	uint64_t e, rip, cs, flags, rsp, ss;
} __attribute__((packed));

// when loading the IDT, they are taken from this table.
// see `sys/idt.c` for details.
extern struct ex_spec const ex_spec_tab[32];

#endif
