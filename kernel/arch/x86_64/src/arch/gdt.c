#include "arch/gdt.h"

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
	struct gdtr gdtr = {
		.size = sizeof(gdt) - 1,
		.base = (uintptr_t)gdt,
	};
	
	gdt_load(&gdtr);
}
