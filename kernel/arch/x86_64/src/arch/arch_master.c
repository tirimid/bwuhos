#include "arch/arch_master.h"

#include "arch/autil.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"

void
arch_master_init(void)
{
	au_println(LT_INFO, "beginning arch master init");
	
	gdt_init();
	pic_init();
	idt_init();
}
