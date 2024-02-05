#include "sys/idt.h"

#include "sys/gdt.h"

#define TA_GATE_INT 0x8e
#define TA_GATE_TRAP 0x8f

struct idt_ent {
	uint16_t base_0;
	uint16_t sel;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t base_1;
	uint32_t base_2;
	uint32_t _zero;
} __attribute__((packed));

static struct idt_ent mk_idt_ent(void *off, uint8_t type_attr);

static struct idt_ent idt[256];

void
idt_init(void)
{
	// it's not ideal to load these at runtime but the performance hit is
	// absolutely negligible.
	
	struct idtr idtr = {
		.size = sizeof(idt) - 1,
		.base = (uintptr_t)idt,
	};
	
	idt_load(&idtr);
}

static struct idt_ent
mk_idt_ent(void *off, uint8_t type_attr)
{
	return (struct idt_ent){
		.base_0 = (uintptr_t)off & 0xffff,
		.base_1 = ((uintptr_t)off & 0xffff0000) >> 16,
		.base_2 = ((uintptr_t)off & 0xffffffff00000000) >> 32,
		.type_attr = type_attr,
		.sel = GS_KERNEL_CODE,
		.ist = 0,
		._zero = 0,
	};
}
