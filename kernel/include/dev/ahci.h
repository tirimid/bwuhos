#ifndef DEV_AHCI_H__
#define DEV_AHCI_H__

#include <stddef.h>
#include <stdint.h>

#include "dev/pci.h"
#include "kdef.h"

// implemented according to AHCI 1.3.1.
// do not use the datastructures directly, only use them through pointer
// indirection to access existing memory in an organized manner.

enum ahci_cmd_hdr_flags {
	ACHF_ATAPI = 0x20,
	ACHF_WRITE = 0x40,
	ACHF_PREFETCH = 0x80,
	ACHF_RESET = 0x100,
	ACHF_BIST = 0x200,
	ACHF_OK_CLR_BUSY = 0x400,
};

struct ahci_port {
	uint32_t cmd_list_base, cmd_list_base_upper;
	uint32_t fis_base, fis_base_upper;
	uint32_t int_status, int_enable;
	uint32_t cmd;
	uint32_t _res_0;
	uint32_t task_file_data;
	uint32_t sig;
	uint32_t sata_status, sata_ctl, sata_err, sata_active;
	uint32_t cmd_issue;
	uint32_t sata_notif;
	uint32_t fis_switch_ctl;
	uint32_t dev_sleep;
	uint32_t _res_1[10];
	uint32_t vendor[4];
} __attribute__((packed));

struct ahci_hba {
	// generic host control.
	uint32_t host_cap;
	uint32_t glob_host_ctl;
	uint32_t int_status;
	uint32_t ports_impl;
	uint32_t ver;
	uint32_t ccc_ctl, ccc_ports;
	uint32_t em_loc, em_ctl;
	uint32_t host_cap_ext;
	uint32_t handoff_ctl_status;
	
	// reserved and vendor-specific registers.
	uint32_t _res[13];
	uint32_t _res_nvmhci[16];
	uint32_t vendor[24];
	
	// HBA port registers.
	struct ahci_port ports[32];
} __attribute__((packed));

struct ahci_recv_fis {
	uint32_t dma_setup_fis[7];
	uint32_t _ignore_0;
	uint32_t pio_setup_fis[5];
	uint32_t _ignore_1[3];
	uint32_t d2h_reg_fis[5];
	uint32_t _ignore_2;
	uint32_t set_dev_bits_fis;
	uint32_t unknown_fis[16];
	uint8_t _res[95];
} __attribute__((packed));

struct ahci_cmd_hdr {
	uint16_t cmd_data, prdt_len;
	uint32_t prd_byte_cnt;
	uint32_t cmd_tab_base, cmd_tab_base_upper;
	uint32_t _res[4];
} __attribute__((packed));

struct ahci_phys_reg_desc {
	uint32_t data_base;
	uint32_t data_base_upper;
	uint32_t _res;
	uint32_t data_byte_cnt_int;
} __attribute__((packed));

struct ahci_cmd_tab {
	uint8_t cmd_fis[64];
	uint8_t atapi_cmd[16];
	uint8_t _res[48];
	struct ahci_phys_reg_desc prd_tab[65535];
} __attribute__((packed));

// `which` is the parameter passed to `pci_conf_find()` to determine which of
// possibly multiple AHCI HBAs in the system to use.
struct ahci_hba *ahci_get_hba_base(size_t which);

struct ahci_recv_fis *ahci_get_recv_fis(struct ahci_port *port);
struct ahci_cmd_hdr *ahci_get_cmd_list(struct ahci_port *port);
struct ahci_cmd_tab *ahci_get_cmd_tab(struct ahci_cmd_hdr *list, uint8_t tab);

int ahci_hba_init(struct ahci_hba *hba);
void ahci_hba_init_pci(struct pci_hdr_00h *hba_hdr);
int ahci_port_init(struct ahci_port *port, struct ahci_hba *hba);
void ahci_port_start_cmd(struct ahci_port *port);
void ahci_port_stop_cmd(struct ahci_port *port);

int ahci_port_rd(phys_addr_t dst, struct ahci_port *port, blk_addr_t src, size_t nsector);
int ahci_port_wr(blk_addr_t dst, struct ahci_port *port, phys_addr_t src, size_t nsector);

#endif
