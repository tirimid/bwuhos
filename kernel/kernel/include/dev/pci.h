#ifndef DEV_PCI_H
#define DEV_PCI_H

#include <stddef.h>
#include <stdint.h>

// datastructures implemented according to PCI local bus 3.0, PCI-to-PCI bridge
// architecture 1.1, had to rely on OSDev wiki for header 02h.

struct pci_hdr_common {
	uint16_t vendor_id, dev_id;
	uint16_t cmd, status;
	uint8_t rev_id;
	uint8_t class_0;
	uint16_t class_1;
	uint8_t cacheline_size, latency_timer, hdr_type, bist;
} __attribute__((packed));

struct pci_hdr_00h {
	struct pci_hdr_common common;
	
	uint32_t bar[6];
	uint32_t p_cardbus_cis;
	uint16_t subsys_vendor_id, subsys_id;
	uint32_t ex_rom_base;
	uint8_t p_cap;
	uint8_t _res_0;
	uint16_t _res_1;
	uint32_t _res_2;
	uint8_t int_line, int_pin, min_gnt, max_lat;
} __attribute__((packed));

struct pci_hdr_01h {
	struct pci_hdr_common common;
	
	uint32_t bar[2];
	uint8_t prim_bus_num, sec_bus_num, sub_bus_num, sec_latency_timer;
	uint8_t io_base, io_limit;
	uint16_t sec_status;
	uint16_t mem_base, mem_limit;
	uint16_t prefetch_mem_base, prefetch_mem_limit;
	uint32_t prefetch_base_upper, prefetch_limit_upper;
	uint16_t io_base_upper, io_limit_upper;
	uint8_t p_cap;
	uint8_t _res_0;
	uint16_t _res_1;
	uint32_t ex_rom_base;
	uint8_t int_line, int_pin;
	uint16_t br_ctl;
} __attribute__((packed));

struct pci_hdr_02h {
	struct pci_hdr_common common;
	
	uint32_t cardbus_sock_exca_base;
	uint8_t cap_list_off, _res;
	uint16_t sec_status;
	uint8_t pci_bus_num, cardbus_bus_num, sub_bus_num, cardbus_latency_timer;
	uint32_t mem_base_0;
	uint32_t mem_limit_0;
	uint32_t mem_base_1;
	uint32_t mem_limit_1;
	uint32_t io_base_0;
	uint32_t io_limit_0;
	uint32_t io_base_1;
	uint32_t io_limit_1;
	uint8_t int_line, int_pin;
	uint16_t br_ctl;
	uint16_t subsys_dev_id, subsys_vendor_id;
	uint32_t pccard_legacy_base;
} __attribute__((packed));

union pci_hdr {
	struct pci_hdr_common common;
	struct pci_hdr_00h type_00h;
	struct pci_hdr_01h type_01h;
	struct pci_hdr_02h type_02h;
};

uint32_t pci_conf_rd_32(uint8_t bus, uint8_t dev, uint8_t fn, uint8_t reg);
void pci_conf_wr_32(uint8_t bus, uint8_t dev, uint8_t fn, uint8_t reg);
int pci_conf_rd_hdr(union pci_hdr *out, uint8_t bus, uint8_t dev, uint8_t fn);
void pci_conf_wr_hdr(uint8_t bus, uint8_t dev, uint8_t fn, union pci_hdr const *hdr);
int pci_conf_find(union pci_hdr *out, uint8_t class, uint8_t subclass, uint8_t prog_if, size_t which);

#endif
