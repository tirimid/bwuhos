#include "dev/ahci.h"

// implement later.
#if 0

#include <stddef.h>

#include "dev/pci.h"
#include "dev/pic.h"
#include "dev/sata_fis.h"
#include "isr/dev_isr.h"
#include "kdef.h"
#include "kutil.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sys/idt.h"

#define DBCI_INT 0x80000000
#define PMPC_C 0x80

static phys_addr_t alloc_phys(struct ahci_hba const *hba);
static int find_free_cmd_slot(struct ahci_port const *port);

int
ahci_hba_init(struct ahci_hba volatile *hba, struct pci_hdr_00h *hba_pci)
{
	ku_log(LT_INFO, "initializing AHCI HBA 0x%x", hba);
	
	// TODO: fix this.
	hba_pci->common.cmd |= 0x2; // set memory space enable.
	hba_pci->common.cmd |= 0x4; // set bus master enable.
	hba_pci->common.cmd &= ~0x400; // clear interrupt disable.
	
	vmm_map_cr3((phys_addr_t)hba, (void *)hba, VF_RW | VF_NO_CACHE);
	vmm_invlpg((void *)hba);
	
	// get BIOS to HBA handoff if BIOS / OS handoff set in host cap 2.
	if (hba->host_cap_ext & 0x1) {
		hba->handoff_ctl_status |= 0x2; // set OS owned semaphore.
		
		while (!(hba->handoff_ctl_status & 0x2)
		       || hba->handoff_ctl_status & 0x1) {
		}
		
		hba->handoff_ctl_status &= ~0x8; // clear OS ownership change.
	}
	
	hba->glob_host_ctl |= 0x1; // set HBA reset.
	while (hba->glob_host_ctl & 0x1)
		;
	
	hba->glob_host_ctl |= 0x2; // set interrupt enable.
	
	// PIC base is 0x20, set ISR and unmask appropriate PIC bit.
	idt_set_isr(0x20 + hba_pci->int_line, (uintptr_t)di_ahci_hba_body, IGT_INT);
	
	uint8_t pic_mask_1, pic_mask_2;
	pic_get_mask(&pic_mask_1, &pic_mask_2);
	
	if (hba_pci->int_line >= 8)
		pic_mask_2 &= ~(hba_pci->int_line - 8);
	else
		pic_mask_1 &= ~hba_pci->int_line;
	
	pic_set_mask(pic_mask_1, pic_mask_2);
	
	for (unsigned i = 0; i < 32; ++i) {
		if (hba->ports_impl & 1 << i) {
			if (ahci_port_init(&hba->ports[i], (struct ahci_hba *)hba)) {
				ku_log(LT_ERR, "failed to initialize AHCI HBA port %u", i);
				return 1;
			}
		}
	}
	
	hba->glob_host_ctl |= 0x80000000; // set AHCI enable.
	
	return 0;
}

int
ahci_port_init(struct ahci_port volatile *port, struct ahci_hba const *hba)
{
	// TODO: free and unmap memory on failure.
	// TODO: check for SATA status and make sure to only init good ports.
	
	ahci_port_stop_cmd(port);
	
	phys_addr_t cmd_list_base;
	if (!(cmd_list_base = alloc_phys(hba)))
		return 1;
	
	vmm_map_cr3(cmd_list_base, (void *)cmd_list_base, VF_RW | VF_NO_CACHE);
	vmm_invlpg((void *)cmd_list_base);
	
	port->cmd_list_base = cmd_list_base & 0xffffffff;
	port->cmd_list_base_upper = cmd_list_base >> 32;
	
	phys_addr_t fis_base;
	if (!(fis_base = alloc_phys(hba)))
		return 1;
	
	vmm_map_cr3(fis_base, (void *)fis_base, VF_RW | VF_NO_CACHE);
	vmm_invlpg((void *)fis_base);
	
	port->fis_base = fis_base & 0xffffffff;
	port->fis_base_upper = fis_base >> 32;
	
	struct ahci_cmd_hdr volatile *cmd_list = (struct ahci_cmd_hdr volatile *)cmd_list_base;
	for (size_t i = 0; i < 32; ++i) {
		phys_addr_t prd_tab_base;
		if (!(prd_tab_base = alloc_phys(hba)))
			return 1;
		
		vmm_map_cr3(prd_tab_base, (void *)prd_tab_base, VF_RW | VF_NO_CACHE);
		vmm_invlpg((void *)prd_tab_base);
		
		cmd_list[i] = (struct ahci_cmd_hdr){
			.cmd_tab_base = prd_tab_base & 0xffffffff,
			.cmd_tab_base_upper = prd_tab_base >> 32,
		};
	}
	
	ahci_port_reset(port);
	
	ahci_port_start_cmd(port);
	
	port->int_enable |= 0x1; // set D2H register FIS interrupt enable.
	port->int_enable |= 0x40000000; // set task file error enable.
	
	ku_log(LT_INFO, "AHCI port 0x%x reports signature 0x%x", port, port->sig);
	
	// identify.
	phys_addr_t id_buf_paddr;
	if (!(id_buf_paddr = alloc_phys(hba)))
		return 1;
	
	vmm_map_cr3(id_buf_paddr, (void *)id_buf_paddr, VF_RW | VF_NO_CACHE);
	
	if (ahci_port_id(port, id_buf_paddr, NULL))
		return 1;
	
	ku_log(LT_INFO, "AHCI port 0x%x identify yields:", port);
	for (size_t i = 0; i < 256; i += 8) {
		uint16_t *id = (uint16_t *)id_buf_paddr + 8 * i;
		ku_log(LT_NONE, "|  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
		       id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
	}
	
	vmm_unmap_cr3((void *)id_buf_paddr);
	pmm_free(id_buf_paddr);
	
	return 0;
}

void
ahci_port_reset(struct ahci_port volatile *port)
{
	// AHCI spec recommends minimum of 1ms delay to ensure hardware has time
	// to read and respond to COMRESET.
	// instead, an arbitrary spin time is used and we just pray it's enough.
	
	port->sata_ctl &= ~0xf; // clear detection / init.
	port->sata_ctl |= 0x1; // set detection / init to 1h.
	
	for (unsigned volatile spin = 0; spin < 1000000; ++spin)
		;
	
	port->sata_ctl &= ~0xf; // clear detection / init.
}

void
ahci_port_start_cmd(struct ahci_port volatile *port)
{
	// spin until status 0h, idle.
	while (port->cmd >> 28)
		;
	
	port->cmd |= 0x10; // set fis receive enable.
	port->cmd |= 0x1; // set clear start (requires FRE).
}

void
ahci_port_stop_cmd(struct ahci_port volatile *port)
{
	port->cmd &= ~0x1; // clear start.
	port->cmd &= ~0x10; // clear fis receive enable.
	
	// spin until command list running and fis receive running clear.
	while (port->cmd & 0x4000 || port->cmd & 0x8000)
		;
}

// TODO: implement asynchronous port operations.

int
ahci_port_id(struct ahci_port volatile *port, phys_addr_t dst, int *marker)
{
	int cmd_slot = find_free_cmd_slot((struct ahci_port *)port);
	if (cmd_slot < 0)
		return 1;
	
	port->int_status = 0xffffffff; // write-clear interrupt status.
	
	phys_addr_t cmd_list_base = port->cmd_list_base;
	cmd_list_base |= (phys_addr_t)port->cmd_list_base_upper << 32;
	struct ahci_cmd_hdr volatile *cmd_list = (struct ahci_cmd_hdr volatile *)cmd_list_base;
	
	struct ahci_cmd_hdr volatile *cmd_hdr = &cmd_list[cmd_slot];
	cmd_hdr->cmd_data = sizeof(struct sata_fis_reg_h2d) / 4;
	cmd_hdr->cmd_data |= 0x80; // set prefetchable.
	cmd_hdr->cmd_data |= 0x400; // set clear busy upon ok.
	cmd_hdr->prd_tab_len = 1;
	
	phys_addr_t cmd_tab_base = cmd_hdr->cmd_tab_base;
	cmd_tab_base = (phys_addr_t)cmd_hdr->cmd_tab_base_upper << 32;
	struct ahci_cmd_tab volatile *cmd_tab = (struct ahci_cmd_tab volatile *)cmd_tab_base;
	
	cmd_tab->prd_tab[0] = (struct ahci_phys_reg_desc){
		.data_base = dst & 0xffffffff,
		.data_base_upper = dst >> 32,
		.data_byte_cnt_int = 511 | DBCI_INT,
	};
	
	struct sata_fis_reg_h2d volatile *cmd_fis = (struct sata_fis_reg_h2d volatile *)cmd_tab->cmd_fis;
	*cmd_fis = (struct sata_fis_reg_h2d){
		.type = SFT_REG_H2D,
		.pm_port_c = PMPC_C,
		.cmd = 0xec, // identity.
		.dev = 0, // master device.
	};
	
	port->cmd_issue |= 1 << cmd_slot;
	
	if (!marker) {
		while (port->cmd_issue & 1 << cmd_slot)
			;
	}
	
	return 0;
}

int
ahci_port_rd(struct ahci_port volatile *port, phys_addr_t dst, blk_addr_t src,
             size_t nsector, int *marker)
{
	// TODO: check if reading too many sectors.
	
	if (!nsector)
		return 0;
	
	int cmd_slot = find_free_cmd_slot((struct ahci_port *)port);
	if (cmd_slot < 0)
		return 1;
	
	port->int_status = 0xffffffff; // write-clear interrupt status.
	
	phys_addr_t cmd_list_base = port->cmd_list_base;
	cmd_list_base |= (phys_addr_t)port->cmd_list_base_upper << 32;
	struct ahci_cmd_hdr volatile *cmd_list = (struct ahci_cmd_hdr volatile *)cmd_list_base;
	
	struct ahci_cmd_hdr volatile *cmd_hdr = &cmd_list[cmd_slot];
	cmd_hdr->cmd_data = sizeof(struct sata_fis_reg_h2d) / 4;
	cmd_hdr->prd_tab_len = 1 + (nsector - 1) / PAGE_SIZE;
	
	phys_addr_t cmd_tab_base = cmd_hdr->cmd_tab_base;
	cmd_tab_base = (phys_addr_t)cmd_hdr->cmd_tab_base_upper << 32;
	struct ahci_cmd_tab volatile *cmd_tab = (struct ahci_cmd_tab volatile *)cmd_tab_base;
	
	// PRDT entries not cleared here, set later anyway.
	// 16 = 128 / 8 = (64 + 16 + 48) / 8.
	ku_fms_64((void *)cmd_tab, 0, 16);
	
	size_t nfinal = nsector;
	for (size_t i = 0; i < cmd_hdr->prd_tab_len - 1; ++i) {
		phys_addr_t data = dst + PAGE_SIZE * i;
		cmd_tab->prd_tab[i] = (struct ahci_phys_reg_desc){
			.data_base = data & 0xffffffff,
			.data_base_upper = data >> 32,
			.data_byte_cnt_int = PAGE_SIZE - 1 | DBCI_INT,
		};
		nfinal -= PAGE_SIZE / SECTOR_SIZE;
	}
	
	// last entry to account for reading a non-page-divisible `nsector`.
	phys_addr_t data_final = dst + PAGE_SIZE * (cmd_hdr->prd_tab_len - 1);
	cmd_tab->prd_tab[cmd_hdr->prd_tab_len - 1] = (struct ahci_phys_reg_desc){
		.data_base = data_final & 0xffffffff,
		.data_base_upper = data_final >> 32,
		.data_byte_cnt_int = SECTOR_SIZE * nfinal - 1 | DBCI_INT,
	};
	
	struct sata_fis_reg_h2d volatile *cmd_fis = (struct sata_fis_reg_h2d volatile *)cmd_tab->cmd_fis;
	*cmd_fis = (struct sata_fis_reg_h2d){
		.type = SFT_REG_H2D,
		.pm_port_c = PMPC_C,
		.cmd = 0x25, // DMA ext. read.
		.lba_low = src & 0xff,
		.lba_mid = src >> 8,
		.lba_high = src >> 16,
		.dev = 0x40, // LBA mode.
		.lba_low_exp = src >> 24,
		.lba_mid_exp = src >> 32,
		.lba_high_exp = src >> 40,
		.nsector = nsector & 0xff,
		.nsector_exp = nsector >> 8,
	};
	
	// wait until busy and data request are clear.
	while (port->task_file_data & 0x80 || port->task_file_data & 0x8)
		;
	
	port->cmd_issue |= 1 << cmd_slot;
	
	if (!marker) {
		while (port->cmd_issue & 1 << cmd_slot) {
			// task file error.
			if (port->int_status & 0x40000000) {
				ku_log(LT_ERR, "failed to AHCI read disk!");
				return 1;
			}
		}
		
		if (port->int_status & 0x40000000) {
			ku_log(LT_ERR, "failed to AHCI read disk!");
			return 1;
		}
	}
	
	return 0;
}

int
ahci_port_wr(struct ahci_port volatile *port, blk_addr_t dst, phys_addr_t src,
             size_t nsector, int *marker)
{
	// TODO: implement AHCI write.
	
	return 1;
}

static phys_addr_t
alloc_phys(struct ahci_hba const *hba)
{
	phys_addr_t paddr = pmm_alloc();
	
	// check that allocated address is compatible with HBA host cap.
	if (hba->host_cap & 0x80000000 && paddr & 0xffffffff00000000) {
		pmm_free(paddr);
		return PHYS_ADDR_NULL;
	}
	
	return paddr;
}

static int
find_free_cmd_slot(struct ahci_port const *port)
{
	for (int i = 0; i < 32; ++i) {
		if (!((port->cmd_issue | port->sata_active) & 1 << i))
			return i;
	}
	
	return -1;
}

#endif
