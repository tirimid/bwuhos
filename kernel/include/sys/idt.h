#ifndef SYS_IDT_H__
#define SYS_IDT_H__

#include <stdint.h>

enum idt_gate_type {
	IGT_INT = 0x8e,
	IGT_TRAP = 0x8f,
};

struct idtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void idt_load(struct idtr const *idtr);
void idt_init(void);

#endif
