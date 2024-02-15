#ifndef ARCH_GDT_H__
#define ARCH_GDT_H__

#include <stdint.h>

#define GA_DPL(dpl) (dpl << 5)

// use `GA_DPL()` macro for privilege level specification.
enum gdt_access {
	GA_A = 0x1,
	GA_RW = 0x2,
	GA_DC = 0x4,
	GA_E = 0x8,
	GA_S = 0x10,
	GA_P = 0x80,
};

enum gdt_flag {
	GF_L = 0x20,
	GF_DB = 0x40,
	GF_G = 0x80,
};

enum gdt_sel {
	GS_KERNEL_NULL = 0x0,
	GS_KERNEL_CODE = 0x8,
	GS_KERNEL_DATA = 0x10,
};

struct gdt_ent {
	uint16_t lim_0;
	uint16_t base_0;
	uint8_t base_1;
	uint8_t access;
	uint8_t lim_1_flags;
	uint8_t base_2;
} __attribute__((packed));

struct gdtr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

void gdt_load(struct gdtr const *gdtr);
void gdt_init(void);

#endif
