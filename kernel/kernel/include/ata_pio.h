#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stddef.h>
#include <stdint.h>

#include "blkdev.h"
#include "kdef.h"
#include "port.h"

enum ata_pio_dev_support {
	APDS_LBA = 0x1,
	APDS_LBA_48 = 0x2,
};

struct ata_pio_dev {
	// drive topology.
	size_t sector_size, nsector;
	
	// ATA data.
	port_t bus_io_port, bus_ctl_port;
	uint8_t dev_num; // 0 or 1 since ATA bus supports two devices.
	uint8_t support;
};

int ata_pio_dev_get(struct ata_pio_dev *out, port_t bus_io_port, port_t bus_ctl_port, uint8_t dev_num);
int ata_pio_dev_id(struct ata_pio_dev const *dev, uint16_t *out_id);
int ata_pio_dev_rd(struct ata_pio_dev const *dev, void *dst, blk_addr_t src, size_t nsector);
int ata_pio_dev_wr(struct ata_pio_dev const *dev, blk_addr_t dst, void const *src, size_t nsector);

// blkdev interface.
struct blkdev ata_pio_blkdev_create(struct ata_pio_dev *dev);
void ata_pio_blkdev_driver_destroy(void *driver_data);
int ata_pio_blkdev_rd(struct blkdev *blkdev, void *dst, blk_addr_t src, size_t n);
int ata_pio_blkdev_wr(struct blkdev *blkdev, blk_addr_t dst, void const *src, size_t n);

#endif
