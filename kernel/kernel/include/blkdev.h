#ifndef BLKDEV_H
#define BLKDEV_H

#include <stddef.h>

#include "kdef.h"

enum blkdev_dev_type {
	BDT_DISK_DRIVE = 0,
};

enum blkdev_driver {
	BD_ATA_PIO = 0,
};

struct blkdev {
	// whichever driver this is a block device over will store its
	// driver-specific data here.
	void *driver_data;
	
	// driver may want some cleanup to be perfomed when the block device is
	// no longer in use - if this is the case, this will be non-`NULL` and
	// needs to be called when the block device's state should be destroyed.
	void (*destroy)(struct blkdev *);
	
	int (*rd)(struct blkdev *, void *, blk_addr_t, size_t);
	int (*wr)(struct blkdev *, blk_addr_t, void const *, size_t);
	
	unsigned char dev_type, driver;
};

void blkdevs_find(void);
struct blkdev const *blkdevs_get(size_t *out_cnt);
struct blkdev *blkdev_get(enum blkdev_dev_type type, enum blkdev_driver driver, size_t which);

#endif
