#ifndef SYS_GDT_H__
#define SYS_GDT_H__

#include <stdint.h>

enum gdt_sel {
	GS_KERNEL_NULL = 0x0,
	GS_KERNEL_CODE = 0x8,
	GS_KERNEL_DATA = 0x10,
};

struct gdtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void gdt_load(struct gdtr const *gdtr);
void gdt_init(void);

#endif
