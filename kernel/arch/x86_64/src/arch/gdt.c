#include "arch/gdt.h"

#include "arch/autil.h"

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

struct gdt_ent {
	uint16_t lim_0;
	uint16_t base_0;
	uint8_t base_1;
	uint8_t access;
	uint8_t lim_1_flags;
	uint8_t base_2;
} __attribute__((packed));

static struct gdt_ent gdt[] = {
	[1] = {
		// kernel code.
		.lim_0 = 0xffff,
		.base_0 = 0x0,
		.base_1 = 0x0,
		.access = GA_E | GA_S | GA_P | GA_DPL(0),
		.lim_1_flags = 0xf | GF_L | GF_G,
		.base_2 = 0x0,
	},
	[2] = {
		// kernel data.
		.lim_0 = 0xffff,
		.base_0 = 0x0,
		.base_1 = 0x0,
		.access = GA_RW | GA_S | GA_P | GA_DPL(0),
		.lim_1_flags = 0xf | GF_G,
		.base_2 = 0x0,
	},
};

void
gdt_init(void)
{
	au_println(LT_INFO, "gdt: init");
	
	struct gdtr gdtr = {
		.size = sizeof(gdt) - 1,
		.base = (uintptr_t)gdt,
	};
	
	gdt_load(&gdtr);
}
