#include <limine.h>

#include "arch/arch_master.h"
#include "dev/fb.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

static void init_other_cpu(struct limine_smp_info *cpu);
static void init_stage_2(void);

static struct limine_smp_request volatile smp_req = {
	.id = LIMINE_SMP_REQUEST,
	.revision = 0,
};

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
	
	for (size_t i = 0; i < smp_req.response->cpu_count; ++i) {
		struct limine_smp_info volatile *cpu = smp_req.response->cpus[i];
		cpu->goto_address = (limine_goto_address)init_other_cpu;
	}
	
	vmm_init(init_stage_2);
}

static void
init_other_cpu(struct limine_smp_info *cpu)
{
	ku_println(LT_INFO, "other CPU jumped to `init_other_cpu()`");
	ku_hang();
}

static void
init_stage_2(void)
{
	ku_println(LT_INFO, "reached kernel init stage 2");
	ku_hang();
}
