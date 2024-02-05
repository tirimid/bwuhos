#include "dev/fb.h"
#include "dev/serial_port.h"
#include "int/pic.h"
#include "kutil.h"
#include "sys/gdt.h"
#include "sys/idt.h"

void
_start(void)
{
	if (sp_init()) {
		// serial port is necessary for debug logging.
		// just hang if init fails.
		// something better should eventually be done instead.
		ku_hang();
	}
	
	fb_init();
	gdt_init();
	pic_init();
	idt_init();
	
	ku_hang();
}
