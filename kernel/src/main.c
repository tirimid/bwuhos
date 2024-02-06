#include "dev/fb.h"
#include "dev/pic.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mlayt.h"
#include "sys/gdt.h"
#include "sys/idt.h"

void
_start(void)
{
	// just hang for now if init fails.
	// obviously this isn't ideal.
	if (sp_init())
		ku_hang();
	
	if (fb_init())
		ku_hang();
	
	gdt_init();
	pic_init();
	idt_init();
	
	if (mlayt_init())
		ku_hang();
	
	ku_log(LT_INFO, "done initializing");
	
	ku_hang();
}
