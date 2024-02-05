#include "dev/fb.h"
#include "int/pic.h"
#include "kutil.h"
#include "sys/gdt.h"
#include "sys/idt.h"

void
_start(void)
{
	fb_init();
	gdt_init();
	pic_init();
	idt_init();
	
	ku_hang();
}
