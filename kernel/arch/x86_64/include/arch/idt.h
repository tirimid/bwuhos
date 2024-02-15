#ifndef ARCH_IDT_H__
#define ARCH_IDT_H__

#include <stddef.h>
#include <stdint.h>

enum idt_gate_type {
	IGT_INT = 0x8e,
	IGT_TRAP = 0x8f,
};

struct idt_ent {
	uint16_t base_0;
	uint16_t sel;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t base_1;
	uint32_t base_2;
	uint32_t _zero;
} __attribute__((packed));

struct idtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void idt_load(struct idtr const *idtr);
void idt_init(void);
void idt_set_isr(size_t vec, uintptr_t addr, uint8_t type_attr);

#endif
