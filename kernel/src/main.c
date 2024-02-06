#include "dev/fb.h"
#include "dev/pic.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "sys/gdt.h"
#include "sys/idt.h"

// delete includes later.
#include "mm/vmm.h"
#include "sys/cpu.h"

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
	if (meml_init())
		ku_hang();
	pmm_init();
	
	ku_log(LT_INFO, "done initializing");
	
	// VMM test.
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	phys_addr_t ppg = pmm_alloc();
	void *vpg0 = 0xffffff90000;
	void *vpg1 = 0xffffff20000;
	
	*(uint32_t *)vpg0 = 0xdeadbeef;
	
	vmm_map(cr.cr3, ppg, vpg0, VF_RW);
	vmm_map(cr.cr3, ppg, vpg1, VF_RW);
	
	*(uint32_t *)vpg0 = 0xdeadbeef;
	
	ku_log(LT_DEBUG, "ppg = 0x%x", ppg);
	ku_log(LT_DEBUG, "vpg0 (0x%x) = 0x%x", vpg0, *(uint32_t *)vpg0);
	ku_log(LT_DEBUG, "vpg1 (0x%x) = 0x%x", vpg1, *(uint32_t *)vpg1);
	
	pmm_free(ppg);
	
	ku_hang();
}
