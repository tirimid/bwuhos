#ifndef BLKDEV_H
#define BLKDEV_H

#include <stddef.h>

#include "kdef.h"

// TODO: add mutexes to lock block devices when destroy / partition operations
// are performed on them.

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
	void (*driver_destroy)(void *);
	
	int (*rd)(struct blkdev *, void *, blk_addr_t, size_t);
	int (*wr)(struct blkdev *, blk_addr_t, void const *, size_t);
	
	// partition information.
	// `part_id` = 0 means block device is not a partition.
	size_t nparts, part_id;
	blk_addr_t part_base;
	size_t part_limit;
	
	unsigned char dev_type, driver;
};

void blkdevs_find(void);
struct blkdev const *blkdevs_get(size_t *out_cnt);
struct blkdev *blkdev_get(enum blkdev_dev_type type, enum blkdev_driver driver, size_t part_id, size_t which);
size_t blkdev_get_which(struct blkdev const *blkdev);
struct blkdev *blkdev_mk_part(struct blkdev *blkdev, blk_addr_t base, size_t limit);

// these wrapper functions should be used instead of directly calling `rd` and
// `wr` from the function pointers in `blkdev`.
void blkdev_destroy(struct blkdev *blkdev);
int blkdev_rd(struct blkdev *blkdev, void *dst, blk_addr_t src, size_t n);
int blkdev_wr(struct blkdev *blkdev, blk_addr_t dst, void const *src, size_t n);

#endif
