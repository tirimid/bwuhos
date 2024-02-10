#include "dev/ahci.h"

#include "dev/pci.h"

struct ahci_hba *
ahci_get_hba_base(size_t which)
{
	struct pci_hdr_00h hdr;
	
	// class = mass storage, subclass = SATA, prog. interface = AHCI.
	if (pci_conf_find(&hdr, 0x1, 0x6, 0x1, which))
		return NULL;
	
	return (struct ahci_hba *)(phys_addr_t)hdr.bar[5];
}

struct ahci_port *
ahci_get_port(struct ahci_hba *hba, uint8_t port)
{
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
ahci_rd(phys_addr_t dst, struct ahci_port *port, blk_addr_t src, size_t nsector)
{
}

int
ahci_wr(blk_addr_t dst, struct ahci_port *port, phys_addr_t src, size_t nsector)
{
}
