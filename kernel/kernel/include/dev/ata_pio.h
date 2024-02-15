#ifndef DEV_ATA_PIO_H__
#define DEV_ATA_PIO_H__

#include <stddef.h>
#include <stdint.h>

#include "kdef.h"

// registers and bits implemented according to OSDev wiki.

enum ata_pio_io_reg {
	APIR_DATA = 0,
	APIR_ERR = 1,
	APIR_FEATS = 1,
	APIR_SECTOR_CNT = 2,
	APIR_SECTOR_NUM = 3,
	APIR_CYL_LOW = 4,
	APIR_CYL_HIGH = 5,
	APIR_DRIVE_SEL = 6,
	APIR_STATUS = 7,
	APIR_CMD = 7,
};

enum ata_pio_ctl_reg {
	APCR_ALT_STATUS = 0,
	APCR_DEV_CTL = 0,
	APCR_DRIVE_ADDR = 1,
};

enum ata_pio_err {
	APE_AMNF = 0x1,
	APE_TKZNF = 0x2,
	APE_ABRT = 0x4,
	APE_MCR = 0x8,
	APE_IDNF = 0x10,
	APE_MC = 0x20,
	APE_UNC = 0x40,
	APE_BBK = 0x80,
};

enum ata_pio_drive_flag {
	APDF_DRV = 0x10,
	APDF_LBA = 0x40,
};

enum ata_pio_status {
	APS_ERR = 0x1,
	APS_IDX = 0x2,
	APS_CORR = 0x4,
	APS_DRQ = 0x8,
	APS_SRV = 0x10,
	APS_DF = 0x20,
	APS_RDY = 0x40,
	APS_BSY = 0x80,
};

enum ata_pio_dev_support {
	APDS_LBA = 0x1,
	APDS_LBA_48 = 0x2,
};

struct ata_pio_dev {
	// drive topology.
	size_t sector_size, nsector;
	
	// ATA data.
	uint16_t bus_io_port, bus_ctl_port;
	uint8_t dev_num; // 0 or 1 since ATA bus supports two devices.
	uint8_t support;
};

int ata_pio_dev_get(struct ata_pio_dev *out, uint16_t bus_io_port, uint16_t bus_ctl_port, uint8_t dev_num);
int ata_pio_dev_id(struct ata_pio_dev const *dev, uint16_t *out_id);
int ata_pio_dev_rd(struct ata_pio_dev const *dev, void *dst, blk_addr_t src, size_t nsector);
int ata_pio_dev_wr(struct ata_pio_dev const *dev, blk_addr_t dst, void const *src, size_t nsector);

#endif
