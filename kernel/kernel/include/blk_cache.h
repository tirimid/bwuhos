#ifndef BLK_CACHE_H
#define BLK_CACHE_H

#include <stddef.h>
#include <stdint.h>

#include "blkdev.h"
#include "kdef.h"

// cached block device reader extension.
// used to provide an efficient byte-level read interface for block devices
// without having to read an arbitrary number of sectors into memory, e.g. when
// reading a large file from disk.
// this is most useful when a lot of reads within the same block are performed,
// reloading the `loaded_blk` and `buf` will call upon the block device
// directly.
struct blk_cache {
	struct blkdev *blkdev;
	uint8_t *buf;
	blk_addr_t loaded_blk;
};

struct blk_cache blk_cache_create(struct blkdev *blkdev);
void blk_cache_destroy(struct blk_cache *cache);
int blk_cache_rd(struct blk_cache *cache, void *dst, uintptr_t addr, size_t n);

#endif
