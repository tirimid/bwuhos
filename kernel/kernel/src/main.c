#include <limine.h>

#include "arch/arch_master.h"
#include "blkdev.h"
#include "fb.h"
#include "kutil.h"
#include "kheap.h"
#include "mbr.h"
#include "mem_layout.h"
#include "pmm.h"
#include "rtc.h"
#include "serial_port.h"
#include "vfs.h"
#include "vmm.h"

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
	
	ku_println(LT_INFO, "main: beginning post-serial-port init");
	
	rtc_init();
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
	ku_println(LT_INFO, "main: other cpu jumped to `init_other_cpu()`");
	ku_hang();
}

static void
init_stage_2(void)
{
	ku_println(LT_INFO, "main: reached init stage 2");
	
	if (kheap_init())
		ku_hang();
	blkdevs_find();
	mbr_find();
	vfs_auto_mount();
	if (vfs_cnt_mounted() == 0) {
		ku_println(LT_ERR, "main: no mounted drives!");
		ku_hang();
	}
	
	// test file read.
	struct vfs_file file;
	vfs_open(&file, "1:misc/bwuhos.md", VFF_READ);
	size_t size;
	vfs_seek(&file, VW_END, 0);
	vfs_abs_tell(&file, &size);
	ku_println(LT_DEBUG, "file is %u bytes long", size);
	char conts[512] = {0};
	vfs_seek(&file, VW_START, 0);
	vfs_rd(&file, conts, size);
	ku_println(LT_DEBUG, "%s", conts);
	vfs_close(&file);
	
	ku_hang();
}
