#include "dev/fb.h"
#include "dev/pic.h"
#include "dev/serial_port.h"
#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sys/gdt.h"
#include "sys/idt.h"

// debug includes.
#include "dev/ata_pio.h"
#include "kdump.h"
#include "sys/port.h"

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
	
	struct ata_pio_dev dev;
	ata_pio_dev_get(&dev, P_ATA_1_IO, P_ATA_1_CTL, 0);
	uint16_t id_buf[256];
	ata_pio_dev_id(&dev, id_buf);
	kdump_ata_id(id_buf);
	
	ku_hang();
}
