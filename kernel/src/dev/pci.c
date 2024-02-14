#include "dev/pci.h"

#include "kutil.h"
#include "sys/port.h"

#define ADDR_ENABLE 0x80000000
#define VENDOR_NONE 0xffff

static size_t hdr_sizes[] = {
	sizeof(struct pci_hdr_00h),
	sizeof(struct pci_hdr_01h),
	sizeof(struct pci_hdr_02h),
};

uint32_t
pci_conf_rd_32(uint8_t bus, uint8_t dev, uint8_t fn, uint8_t reg)
{
	uint32_t addr = ADDR_ENABLE;
	addr |= (uint32_t)bus << 16;
	addr |= (uint32_t)dev << 11;
	addr |= (uint32_t)fn << 8;
	addr |= reg;
	addr &= 0x80fffffc; // clear reserved bits.
	
	port_wr_32(P_PCI_CONF_ADDR, addr);
	return port_rd_32(P_PCI_CONF_DATA);
}

void
pci_conf_wr_32(uint8_t bus, uint8_t dev, uint8_t fn, uint8_t reg)
{
}

int
pci_conf_rd_hdr(union pci_hdr *out, uint8_t bus, uint8_t dev, uint8_t fn)
{
	uint32_t vendor_id_dev_id = pci_conf_rd_32(bus, dev, fn, 0);
	if ((vendor_id_dev_id & 0xffff) == VENDOR_NONE)
		return 1;
	
	// read common before determining full PCI header size.
	*(uint32_t *)out = vendor_id_dev_id;
	for (size_t reg = 4; reg < sizeof(struct pci_hdr_common); reg += 4)
		*((uint32_t *)out + reg / 4) = pci_conf_rd_32(bus, dev, fn, reg);
	
	// read type-specific PCI header.
	struct pci_hdr_common const *common = &out->common;
	size_t hdr_size = hdr_sizes[common->hdr_type & 0x7f];
	
	for (size_t reg = sizeof(struct pci_hdr_common); reg < hdr_size; reg += 4)
		*((uint32_t *)out + reg / 4) = pci_conf_rd_32(bus, dev, fn, reg);
	
	return 0;
}

void
pci_conf_wr_hdr(union pci_hdr *out, uint8_t type, uint8_t bus, uint8_t dev,
                uint8_t fn)
{
}

int
pci_conf_find(union pci_hdr *out, uint8_t class, uint8_t subclass,
              uint8_t prog_if, size_t which)
{
	uint8_t buf[256];
	size_t nfound = 0;
	for (unsigned bus = 0; bus < 256; ++bus) {
		for (unsigned dev = 0; dev < 32; ++dev) {
			if (pci_conf_rd_hdr((union pci_hdr *)buf, bus, dev, 0))
				continue;
			
			struct pci_hdr_common const *common = (struct pci_hdr_common *)buf;
			if (common->class_0 == prog_if
			    && (common->class_1 & 0xff) == subclass
			    && common->class_1 >> 8 == class
			    && ++nfound > which) {
				ku_smc_8(out, buf, hdr_sizes[common->hdr_type & 0x7f]);
				return 0;
			}
			
			// check multi-function devices.
			if (!(common->hdr_type & 0x80))
				continue;
			
			for (unsigned fn = 1; fn < 8; ++fn) {
				if (pci_conf_rd_hdr((union pci_hdr *)buf, bus, dev, fn))
					continue;
				
				common = (struct pci_hdr_common *)buf;
				if (common->class_0 == prog_if
				    && (common->class_1 & 0xff) == subclass
				    && common->class_1 >> 8 == class
				    && ++nfound > which) {
					ku_smc_8(out, buf, hdr_sizes[common->hdr_type & 0x7f]);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
