#include "arch/arch_master.h"
#include "dev/fb.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

static void init_stage_2(void);

void
_start(void)
{
	if (sp_init())
		ku_hang();
	
	if (fb_init())
		ku_hang();
	
	if (meml_init())
		ku_hang();
	
	pmm_init();
	arch_master_init();
	vmm_init(init_stage_2);
}

static void
init_stage_2(void)
{
	ku_println(LT_INFO, "reached kernel init stage 2");
	ku_hang();
}
