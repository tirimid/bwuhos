#include "dev/fb.h"
#include "dev/pic.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mlayt.h"
#include "mm/pfa.h"
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
	pfa_init();
	
	ku_log(LT_INFO, "done initializing");
	
	for (size_t i = 0; i < 15; ++i) {
		paddr_t addr = pfa_alloc();
		ku_log(LT_INFO, "allocated 0x%x", addr);
		pfa_free(addr);
	}
	
	ku_hang();
}
