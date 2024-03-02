#include "dev/ata_pio.h"

#include "dev/blkdev.h"
#include "katomic.h"
#include "kutil.h"
#include "mm/kheap.h"

// max number of ATA PIO buses that can be cached.
// more buses than this can exist and should work, but duplicate work will be
// performed on selecting them.
#define MAX_BUSES 4

// used solely for cached device selection.
struct bus_info {
	port_t io_port;
	uint8_t sel_dev_num, sel_data;
};

static void sel_drive(port_t bus_io_port, uint8_t dev_num, uint8_t sel_data);

static struct bus_info bus_cache[MAX_BUSES];
static size_t bus_cache_size = 0;
static k_mutex_t mutex;

int
ata_pio_dev_get(struct ata_pio_dev *out, port_t bus_io_port,
                port_t bus_ctl_port, uint8_t dev_num)
{
	out->bus_io_port = bus_io_port,
	out->bus_ctl_port = bus_ctl_port,
	out->dev_num = dev_num,
	out->support = 0,
	
	k_mutex_lock(&mutex);
	
	sel_drive(bus_io_port, dev_num, 0xa | dev_num);
	
	// initial ID to determine device information.
	// since support is not yet determined, we cannot know whether to write
	// the 16 bit or 8 bit port registers.
	// so, just write both for now, and save result in support.
	port_wr(bus_io_port + APIR_SECTOR_CNT, 0, PS_16);
	port_wr(bus_io_port + APIR_SECTOR_NUM, 0, PS_16);
	port_wr(bus_io_port + APIR_CYL_LOW, 0, PS_16);
	port_wr(bus_io_port + APIR_CYL_HIGH, 0, PS_16);
	port_wr(bus_io_port + APIR_SECTOR_CNT, 0, PS_8);
	port_wr(bus_io_port + APIR_SECTOR_NUM, 0, PS_8);
	port_wr(bus_io_port + APIR_CYL_LOW, 0, PS_8);
	port_wr(bus_io_port + APIR_CYL_HIGH, 0, PS_8);
	
	ku_spin_cycles(100);
	
	port_wr(bus_io_port + APIR_CMD, 0xec, PS_8); // IDENTIFY.
	uint8_t status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	if (!status) {
		k_mutex_unlock(&mutex);
		return 1;
	}
	
	// TODO: check for non-standard ATAPI and give error if found.
	while (status & APS_BSY)
		status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	
	while (!(status & APS_DRQ) && !(status & APS_ERR))
		status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	
	if (status & APS_ERR) {
		k_mutex_unlock(&mutex);
		return 1;
	}
	
	uint16_t id_buf[256];
	for (size_t i = 0; i < 256; ++i)
		id_buf[i] = port_rd(bus_io_port + APIR_DATA, PS_16);
	
	out->sector_size = id_buf[51];
	out->nsector = id_buf[60] | (size_t)id_buf[61] << 16;
	
	out->support |= (APDS_LBA | APDS_LBA_48) * !!(id_buf[83] & 0x400);
	out->support |= APDS_LBA * !!out->nsector;
	
	if (!(out->support & APDS_LBA)) {
		ku_println(LT_ERR, "ata_pio: device at 0x%x (p) uses CHS which is unsupported!", bus_io_port);
		k_mutex_unlock(&mutex);
		return 1;
	}
	
	k_mutex_unlock(&mutex);
	return 0;
}

int
ata_pio_dev_id(struct ata_pio_dev const *dev, uint16_t *out_id)
{
	k_mutex_lock(&mutex);
	
	sel_drive(dev->bus_io_port, dev->dev_num, 0xa | dev->dev_num);
	
	enum port_size ps = dev->support & APDS_LBA_48 ? PS_16 : PS_8;
	port_wr(dev->bus_io_port + APIR_SECTOR_CNT, 0, ps);
	port_wr(dev->bus_io_port + APIR_SECTOR_NUM, 0, ps);
	port_wr(dev->bus_io_port + APIR_CYL_LOW, 0, ps);
	port_wr(dev->bus_io_port + APIR_CYL_HIGH, 0, ps);
	
	ku_spin_cycles(100);
	
	port_wr(dev->bus_io_port + APIR_CMD, 0xec, PS_8); // IDENTIFY.
	
	uint8_t status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	// TODO: check for non-standard ATAPI and give error if found.
	while (status & APS_BSY)
		status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	while (!(status & APS_DRQ) && !(status & APS_ERR))
		status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	if (status & APS_ERR) {
		k_mutex_unlock(&mutex);
		return 1;
	}
	
	for (size_t i = 0; i < 256; ++i)
		out_id[i] = port_rd(dev->bus_io_port + APIR_DATA, PS_16);
	
	k_mutex_unlock(&mutex);
	return 0;
}

int
ata_pio_dev_rd(struct ata_pio_dev const *dev, void *dst, blk_addr_t src,
               size_t nsector)
{
	if (!nsector)
		return 0;
	
	size_t max_sectors = dev->support & APDS_LBA_48 ? 0x10000 : 0x100;
	if (nsector > max_sectors)
		return 1;
	
	// for convenience, `nsector` is wrapped.
	// that way you don't need to worry about adjusting for max read size
	// being at `nsector` = 0.
	nsector = nsector == max_sectors ? 0 : nsector;
	
	k_mutex_lock(&mutex);
	
	if (dev->support & APDS_LBA_48) {
		sel_drive(dev->bus_io_port, dev->dev_num, 0xe0 | dev->dev_num << 4);
		port_wr(dev->bus_io_port + APIR_SECTOR_CNT, nsector >> 8, PS_8);
		port_wr(dev->bus_io_port + APIR_SECTOR_NUM, src >> 24, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_LOW, src >> 32, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_HIGH, src >> 40, PS_8);
		port_wr(dev->bus_io_port + APIR_SECTOR_CNT, nsector, PS_8);
		port_wr(dev->bus_io_port + APIR_SECTOR_NUM, src, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_LOW, src >> 8, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_HIGH, src >> 16, PS_8);
		port_wr(dev->bus_io_port + APIR_CMD, 0x24, PS_8); // READ EXT.
	} else {
		sel_drive(dev->bus_io_port, dev->dev_num, 0x40 | dev->dev_num << 4);
		port_wr(dev->bus_io_port + APIR_SECTOR_CNT, nsector, PS_8);
		port_wr(dev->bus_io_port + APIR_SECTOR_NUM, src, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_LOW, src >> 8, PS_8);
		port_wr(dev->bus_io_port + APIR_CYL_HIGH, src >> 16, PS_8);
		port_wr(dev->bus_io_port + APIR_CMD, 0x20, PS_8); // READ.
	}
	
	for (size_t i = 0; i < nsector; ++i) {
		uint8_t status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
		while (!(status & APS_DRQ) && !(APS_ERR))
			status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
		
		if (status & APS_ERR) {
			ku_println(LT_ERR, "ata_pio: device at 0x%x (p) experienced a read failure at sector 0x%x + 0x%x!", dev->bus_io_port, src, i);
			k_mutex_unlock(&mutex);
			return 1;
		}
		
		for (size_t j = 0; j < dev->sector_size / 2; ++j)
			*((uint16_t *)dst + j) = port_rd(dev->bus_io_port + APIR_DATA, PS_16);
		
		// 400ns delay is suggested.
		// instead, I'm just going to wing it and hope shit doesn't die.
		ku_spin_cycles(1000);
	}
	
	k_mutex_unlock(&mutex);
	return 0;
}

int
ata_pio_dev_wr(struct ata_pio_dev const *dev, blk_addr_t dst, void const *src,
               size_t nsector)
{
	k_mutex_lock(&mutex);
	
	// TODO: implement ATA PIO write.
	
	k_mutex_unlock(&mutex);
	return 1;
}

struct blkdev
ata_pio_blkdev_create(size_t blkdev_id, struct ata_pio_dev *dev)
{
	struct blkdev blkdev = {
		.destroy = ata_pio_blkdev_destroy,
		.rd = ata_pio_blkdev_rd,
		.wr = ata_pio_blkdev_wr,
		.dev_type = BDT_DISK_DRIVE,
		.driver = BD_ATA_PIO,
	};
	blkdev.driver_data = kheap_alloc(sizeof(*dev));
	*(struct ata_pio_dev *)blkdev.driver_data = *dev;
	
	return blkdev;
}

void
ata_pio_blkdev_destroy(struct blkdev *blkdev)
{
	kheap_free(blkdev->driver_data);
}

int
ata_pio_blkdev_rd(struct blkdev *blkdev, void *dst, blk_addr_t src, size_t n)
{
	return ata_pio_dev_rd(blkdev->driver_data, dst, src, n);
}

int
ata_pio_blkdev_wr(struct blkdev *blkdev, blk_addr_t dst, void const *src,
                  size_t n)
{
	return ata_pio_dev_wr(blkdev->driver_data, dst, src, n);
}

static void
sel_drive(port_t bus_io_port, uint8_t dev_num, uint8_t sel_data)
{
	size_t which;
	for (which = 0; which < bus_cache_size; ++which) {
		if (bus_cache[which].io_port == bus_io_port)
			break;
	}
	
	if (which < bus_cache_size
	    && bus_cache[which].sel_dev_num == dev_num
	    && bus_cache[which].sel_data == sel_data) {
		return;
	}
	
	if (which < MAX_BUSES && which == bus_cache_size) {
		bus_cache[bus_cache_size++] = (struct bus_info){
			.io_port = bus_io_port,
			.sel_dev_num = dev_num,
			.sel_data = sel_data,
		};
		
		port_wr(bus_io_port + APIR_DRIVE_SEL, sel_data, PS_8);
		ku_spin_cycles(1000);
		
		return;
	}
	
	if (bus_cache[which].sel_dev_num != dev_num
	    || bus_cache[which].sel_data != sel_data
	    || which == MAX_BUSES) {
		if (which < MAX_BUSES) {
			bus_cache[which].sel_dev_num = dev_num;
			bus_cache[which].sel_data = sel_data;
		}
		
		port_wr(bus_io_port + APIR_DRIVE_SEL, sel_data, PS_8);
		ku_spin_cycles(1000);
	}
}
