#ifndef SYS_IDT_H__
#define SYS_IDT_H__

#include <stdint.h>

enum gdt_sel {
	GS_KERNEL_NULL = 0x0,
	GS_KERNEL_CODE = 0x8,
	GS_KERNEL_DATA = 0x10,
};

struct idtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void idt_load(struct idtr const *idtr);
void idt_init(void);

#endif
