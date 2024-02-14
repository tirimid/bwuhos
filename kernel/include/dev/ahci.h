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
	uint16_t cmd_data, prd_tab_len;
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

// i can't do this shit anymore, implement later.
#if 0
int ahci_hba_init(struct ahci_hba volatile *hba, struct pci_hdr_00h *hba_pci);
int ahci_port_init(struct ahci_port volatile *port, struct ahci_hba const *hba);
void ahci_port_reset(struct ahci_port volatile *port);
void ahci_port_start_cmd(struct ahci_port volatile *port);
void ahci_port_stop_cmd(struct ahci_port volatile *port);

// if non-`NULL`, `marker` will be set to 0 upon successful request, 1 upon
// successful request completion, and -1 upon failed request completion - this
// is for use in asynchronous code.
// if `NULL`, the calls will simply block until the request completes, at which
// point a return value of 0 indicates success and non-zero indicates failure.
int ahci_port_id(struct ahci_port volatile *port, phys_addr_t dst, int *marker);
int ahci_port_rd(struct ahci_port volatile *port, phys_addr_t dst, blk_addr_t src, size_t nsector, int *marker);
int ahci_port_wr(struct ahci_port volatile *port, blk_addr_t dst, phys_addr_t src, size_t nsector, int *marker);
#endif

#endif
