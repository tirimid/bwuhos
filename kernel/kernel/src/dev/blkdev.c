#include "dev/blkdev.h"

#include "dev/ata_pio.h"
#include "kutil.h"
#include "sys/port.h"

#define MAX_BLKDEV_CNT 64

static int register_blkdev(struct blkdev *blkdev);

static struct blkdev blkdevs[MAX_BLKDEV_CNT];
static size_t blkdev_cnt;

void
blkdevs_find(void)
{
	ku_println(LT_INFO, "blkdev: find");
	
	blkdev_cnt = 0;
	
	// scan likely ATA PIO ports.
	static port_t const ata_pio_io[] = {0x1f0, 0x170, 0x1e8, 0x168};
	static port_t const ata_pio_ctl[] = {0x3f6, 0x376, 0x3e6, 0x366};
	for (size_t i = 0; i < sizeof(ata_pio_io) / sizeof(port_t); ++i) {
		for (int j = 0; j < 2; ++j) {
			struct ata_pio_dev dev;
			if (ata_pio_dev_get(&dev, ata_pio_io[i], ata_pio_ctl[i], j))
				continue;
			
			struct blkdev blkdev = {
				.name = {0},
				.destroy = ata_pio_blkdev_destroy,
				.rd = ata_pio_blkdev_rd,
				.wr = ata_pio_blkdev_wr,
			};
			
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
blkdev_get(char const *name)
{
}

static int
register_blkdev(struct blkdev *blkdev)
{
	if (blkdev_cnt >= MAX_BLKDEV_CNT) {
		ku_println(LT_WARN, "blkdev: unable to register device - %s", blkdev->name);
		return 1;
	}
	
	blkdevs[blkdev_cnt++] = *blkdev;
	return 0;
}
