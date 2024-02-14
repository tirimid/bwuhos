#include "sys/idt.h"

#include "isr/exception.h"
#include "kutil.h"
#include "sys/gdt.h"

struct idt_ent {
	uint16_t base_0;
	uint16_t sel;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t base_1;
	uint32_t base_2;
	uint32_t _zero;
} __attribute__((packed));

static struct idt_ent mk_idt_ent(uintptr_t addr, uint8_t type_attr);

static struct idt_ent idt[256];

void
idt_init(void)
{
	ku_log(LT_INFO, "initializing IDT");
	
	// it's not ideal to load these ISRs at runtime but the performance hit
	// is absolutely negligible.
	for (size_t i = 0; i < 32; ++i) {
		struct ex_spec const *es = &ex_spec_tab[i];
		idt_set_isr(i, es->addr, es->gate);
	}
	
	struct idtr idtr = {
		.size = sizeof(idt) - 1,
		.base = (uintptr_t)idt,
	};
	
	idt_load(&idtr);
}

void
idt_set_isr(size_t vec, uintptr_t addr, uint8_t type_attr)
{
	idt[vec] = mk_idt_ent(addr, type_attr);
}

static struct idt_ent
mk_idt_ent(uintptr_t addr, uint8_t type_attr)
{
	return (struct idt_ent){
		.base_0 = addr & 0xffff,
		.base_1 = (addr & 0xffff0000) >> 16,
		.base_2 = (addr & 0xffffffff00000000) >> 32,
		.type_attr = type_attr,
		.sel = GS_KERNEL_CODE,
		.ist = 0,
		._zero = 0,
	};
}
