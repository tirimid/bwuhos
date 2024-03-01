#ifndef DEV_BLKDEV_H
#define DEV_BLKDEV_H

#include <stddef.h>

#include "kdef.h"

struct blkdev {
	// the kernel will attempt to give every block device a unique name that
	// indicates what kind of device it is and what driver it is using:
	// `dd/atapio/...` - disk drive, ATA PIO driver.
	char name[32];
	
	// whichever driver this is a block device over will store its
	// driver-specific data here.
	void *driver_data;
	
	// driver may want some cleanup to be perfomed when the block device is
	// no longer in use - if this is the case, this will be non-`NULL` and
	// needs to be called when the block device's state should be destroyed.
	void (*destroy)(struct blkdev *);
	
	int (*rd)(struct blkdev *, void *, blk_addr_t, size_t);
	int (*wr)(struct blkdev *, blk_addr_t, void const *, size_t);
	
	// TODO: functions for block device statistics requests.
};

void blkdevs_find(void);
struct blkdev const *blkdevs_get(size_t *out_cnt);
struct blkdev *blkdev_get(char const *name);

#endif
