#include "dev/fb.h"
#include "dev/pic.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sys/gdt.h"
#include "sys/idt.h"

// for testing, remove later.
#include "sys/cpu.h"

static void init_stage_1(void);
static void init_stage_2(void);

void
_start(void)
{
	init_stage_1();
}

static void
init_stage_1(void)
{
	ku_log(LT_INFO, "reached kernel init stage 1");
	
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
	vmm_init(init_stage_2);
}

static void
init_stage_2(void)
{
	ku_log(LT_INFO, "reached kernel init stage 2");
	
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	phys_addr_t ppg = pmm_alloc();
	
	void *vpg0 = (void *)0x800f000f000;
	void *vpg1 = (void *)0x800f000d000;
	
	vmm_map(cr.cr3, ppg, vpg0, VF_RW);
	vmm_map(cr.cr3, ppg, vpg1, VF_RW);
	vmm_invlpg(vpg0);
	vmm_invlpg(vpg1);
	
	*(uint32_t *)vpg0 = 0xdeadbeef;
	
	ku_log(LT_DEBUG, "ppg    0x%x", ppg);
	ku_log(LT_DEBUG, "vpg0   0x%x", vpg0);
	ku_log(LT_DEBUG, "*vpg0  0x%x", *(uint32_t *)vpg0);
	ku_log(LT_DEBUG, "vpg1   0x%x", vpg1);
	ku_log(LT_DEBUG, "*vpg1  0x%x", *(uint32_t *)vpg1);
	
	pmm_free(ppg);
	
	ku_hang();
}
