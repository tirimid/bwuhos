#include "dev/ahci.h"

#include "dev/pci.h"
#include "isr/dev_isr.h"
#include "kutil.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sys/idt.h"

static phys_addr_t alloc_phys(struct ahci_hba const *hba);

struct ahci_hba *
ahci_get_hba_base(size_t which)
{
	union pci_hdr hdr;
	
	// class = mass storage, subclass = SATA, prog. interface = AHCI.
	if (pci_conf_find(&hdr, 0x1, 0x6, 0x1, which))
		return NULL;
	
	return (struct ahci_hba *)(phys_addr_t)hdr.type_00h.bar[5];
}

struct ahci_recv_fis *
ahci_get_recv_fis(struct ahci_port *port)
{
}

struct ahci_cmd_hdr *
ahci_get_cmd_list(struct ahci_port *port)
{
}

struct ahci_cmd_tab *
ahci_get_cmd_tab(struct ahci_cmd_hdr *list, uint8_t tab)
{
}

int
ahci_hba_init(struct ahci_hba *hba)
{
	ku_log(LT_INFO, "initializing AHCI HBA 0x%x", hba);
	
	vmm_map_cr3((phys_addr_t)hba, hba, VF_RW | VF_NO_CACHE);
	vmm_invlpg(hba);
	
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
	
	for (unsigned i = 0; i < 32; ++i) {
		if (hba->ports_impl & 1 << i) {
			if (ahci_port_init(&hba->ports[i], hba)) {
				ku_log(LT_ERR, "failed to initialize AHCI HBA port %u", i);
				return 1;
			}
		}
	}
	
	hba->glob_host_ctl |= 0x80000000; // set AHCI enable.
	hba->glob_host_ctl |= 0x2; // set interrupt enable.
	
	return 0;
}

void
ahci_hba_init_pci(struct pci_hdr_00h *hba_hdr)
{
	ku_log(LT_INFO, "initializing AHCI HBA PCI header 0x%x", hba_hdr);
	
	hba_hdr->common.cmd |= 0x2; // set memory space enable.
	hba_hdr->common.cmd |= 0x4; // set bus master enable.
	hba_hdr->common.cmd &= ~0x400; // clear interrupt disable.
	
	idt_set_isr(hba_hdr->int_line, (uintptr_t)di_ahci_hba_head, IGT_INT);
}

int
ahci_port_init(struct ahci_port *port, struct ahci_hba *hba)
{
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
	
	// TODO: generate command tables.
	
	return 0;
}

void
ahci_port_start_cmd(struct ahci_port *port)
{
	// spin until status 0h, idle.
	while (port->cmd >> 28)
		;
}

void
ahci_port_stop_cmd(struct ahci_port *port)
{
	port->cmd &= ~0x1; // clear start.
	port->cmd &= ~0x10; // clear fis receive enable.
	
	// spin until command list running and fis receive running clear.
	while (port->cmd & 0x4000 || port->cmd & 0x8000)
		;
}

int
ahci_port_rd(phys_addr_t dst, struct ahci_port *port, blk_addr_t src,
             size_t nsector)
{
}

int
ahci_port_wr(blk_addr_t dst, struct ahci_port *port, phys_addr_t src,
             size_t nsector)
{
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
