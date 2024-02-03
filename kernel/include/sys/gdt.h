#ifndef SYS_GDT_H__
#define SYS_GDT_H__

#include <stdint.h>

struct gdtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void gdt_load(struct gdtr const *gdtr);
void gdt_init(void);

#endif
