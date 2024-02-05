#ifndef INT_EXCEPTION_H__
#define INT_EXCEPTION_H__

#include <stdint.h>

struct exception_spec {
	uintptr_t addr;
	uint8_t gate;
};

// when loading the IDT, they are taken from this table.
// see `sys/idt.c` for details.
extern struct exception_spec const exception_spec_tab[32];

#endif
