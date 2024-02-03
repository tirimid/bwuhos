#include "sys/gdt.h"

#define GA_DPL(dpl) (dpl << 5)

// use `GA_DPL()` macro for privilege level specification.
enum gdt_access {
	GA_ACCESSED = 0x1,
	GA_RW = 0x2,
	GA_DIR_CONF = 0x4,
	GA_EXEC = 0x8,
	GA_TYPE = 0x10,
	GA_PRESENT = 0x80,
};

enum gdt_flag {
	GF_LONG_CODE = 0x20,
	GF_SIZE = 0x40,
	GF_GRAN = 0x80,
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

static struct gdt_ent gdt[] = {
	{
		// kernel null.
	},
	{
		// kernel code.
		.lim_0 = 0xffff,
		.base_0 = 0x0,
		.base_1 = 0x0,
		.access = GA_EXEC | GA_TYPE | GA_PRESENT | GA_DPL(0),
		.lim_1_flags = 0xf | GF_LONG_CODE | GF_GRAN,
		.base_2 = 0x0,
	},
	{
		// kernel data.
		.lim_0 = 0xffff,
		.base_0 = 0x0,
		.base_1 = 0x0,
		.access = GA_RW | GA_TYPE | GA_PRESENT | GA_DPL(0),
		.lim_1_flags = 0xf | GF_GRAN,
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
