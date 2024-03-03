#ifndef BLKDEV_H
#define BLKDEV_H

#include <stddef.h>
#include <stdint.h>

#include "kdef.h"

#define BLKDEV_MAX_PARTS 32

// TODO: add mutex locking to protect block devices.

enum blkdev_dev_type {
	BDT_NULL = 0,
	BDT_DISK_DRIVE,
};

enum blkdev_driver {
	BD_NULL = 0,
	BD_ATA_PIO,
};

enum blkdev_flag {
	BF_PART = 0x1,
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
	
	struct blkdev *children; // allocated to max size upon creation.
	size_t nchildren;
	blk_addr_t part_base;
	uint8_t flags;
	
	unsigned char dev_type, driver;
	size_t blk_size, nblk;
};

void blkdevs_find(void);
struct blkdev *blkdev_register(struct blkdev *blkdev);
void blkdev_unregister(struct blkdev *blkdev);
struct blkdev *blkdevs_get(size_t *out_cnt);
struct blkdev *blkdev_get(enum blkdev_dev_type type, enum blkdev_driver driver, size_t which);
struct blkdev blkdev_create(void *driver_data, void (*driver_destroy)(void *), int (*rd)(struct blkdev *, void *, blk_addr_t, size_t), int (*wr)(struct blkdev *, blk_addr_t, void const *, size_t), enum blkdev_dev_type type, enum blkdev_driver driver, uint8_t flags, size_t blk_size, size_t nblk);
struct blkdev *blkdev_mk_part(struct blkdev *blkdev, blk_addr_t base, size_t limit);
void blkdev_rm_part(struct blkdev *blkdev, size_t which);
size_t blkdev_get_which(struct blkdev const *blkdev);

// these wrapper functions should be used instead of directly calling `rd` and
// `wr` from the function pointers in `blkdev`.
void blkdev_destroy(struct blkdev *blkdev);
int blkdev_rd(struct blkdev *blkdev, void *dst, blk_addr_t src, size_t n);
int blkdev_wr(struct blkdev *blkdev, blk_addr_t dst, void const *src, size_t n);

#endif
