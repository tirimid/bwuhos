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
#include "dev/ahci.h"
#include "dev/pci.h"

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
	
	union pci_hdr hba_pci;
	pci_conf_find(&hba_pci, 0x1, 0x6, 0x1, 0);
	struct ahci_hba *hba = (struct ahci_hba *)hba_pci.type_00h.bar[5];
	
	ahci_hba_init(hba, &hba_pci.type_00h);
	phys_addr_t dst = pmm_alloc();
	vmm_map_cr3(dst, (void *)dst, VF_RW | VF_NO_CACHE);
	ku_fms_64((void *)dst, 0, PAGE_SIZE / 8);
	ahci_port_rd(&hba->ports[0], dst, 0, 1);
	
	ku_log(LT_DEBUG, "read 0x%x from LBA 0", *(uint64_t *)dst);
	
	ku_hang();
}
