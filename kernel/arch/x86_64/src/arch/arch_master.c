#include "arch/arch_master.h"

#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"

void
arch_master_init(void)
{
	gdt_init();
	pic_init();
	idt_init();
}
