#include "dev/blkdev.h"

#include "dev/ata_pio.h"
#include "kutil.h"
#include "sys/port.h"

#define MAX_BLKDEV_CNT 64

static int register_blkdev(struct blkdev *blkdev);
static char const *blkdev_info_str(struct blkdev const *blkdev);

static struct blkdev blkdevs[MAX_BLKDEV_CNT];
static size_t blkdev_cnt;

void
blkdevs_find(void)
{
	ku_println(LT_INFO, "blkdev: finding devices");
	
	blkdev_cnt = 0;
	
	// scan likely ATA PIO ports.
	static port_t const ata_pio_io[] = {0x1f0, 0x170};
	static port_t const ata_pio_ctl[] = {0x3f6, 0x376};
	for (size_t i = 0; i < sizeof(ata_pio_io) / sizeof(port_t); ++i) {
		for (int j = 0; j < 2; ++j) {
			struct ata_pio_dev dev;
			if (ata_pio_dev_get(&dev, ata_pio_io[i], ata_pio_ctl[i], j))
				continue;
			
			struct blkdev blkdev = ata_pio_blkdev_create(blkdev_cnt, &dev);
			if (register_blkdev(&blkdev))
				return;
		}
	}
}

struct blkdev const *
blkdevs_get(size_t *out_cnt)
{
	*out_cnt = blkdev_cnt;
	return blkdevs;
}

struct blkdev *
blkdev_get(enum blkdev_dev_type type, enum blkdev_driver driver, size_t which)
{
	for (size_t i = 0; i < blkdev_cnt; ++i) {
		if (blkdevs[i].dev_type == type
		    && blkdevs[i].driver == driver
		    && which-- == 0) {
			return &blkdevs[i];
		}
	}
	
	return NULL;
}

static int
register_blkdev(struct blkdev *blkdev)
{
	if (blkdev_cnt >= MAX_BLKDEV_CNT) {
		ku_println(LT_WARN, "blkdev: unable to register - %s", blkdev_info_str(blkdev));
		return 1;
	}
	
	ku_println(LT_INFO, "blkdev: registered %s", blkdev_info_str(blkdev));
	blkdevs[blkdev_cnt++] = *blkdev;
	return 0;
}

static char const *
blkdev_info_str(struct blkdev const *blkdev)
{
	// printable format summarizing most important identifying details of a
	// block device to the kernel (device type and driver).
	switch (blkdev->dev_type) {
	case BDT_DISK_DRIVE:
		switch (blkdev->driver) {
		case BD_ATA_PIO:
			return "dd:atapio";
		default:
			return "dd:null";
		}
	default:
		return "null:null";
	}
}
