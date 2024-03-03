#include "blk_cache.h"

#include "kheap.h"
#include "kutil.h"

struct blk_cache
blk_cache_create(struct blkdev *blkdev)
{
	return (struct blk_cache){
		.blkdev = blkdev,
		.buf = kheap_alloc(blkdev->blk_size),
		.loaded_blk = blkdev->nblk,
	};
}

void
blk_cache_destroy(struct blk_cache *cache)
{
	kheap_free(cache->buf);
}

int
blk_cache_rd(struct blk_cache *cache, void *dst, uintptr_t addr, size_t n)
{
	if (!n)
		return 0;
	
	struct blkdev *blkdev = cache->blkdev;
	
	blk_addr_t lba_lb = addr / blkdev->blk_size;
	blk_addr_t lba_ub = (addr + n - 1) / blkdev->blk_size;
	uintptr_t byte_addr = addr % blkdev->blk_size;
	
	// reading across block boundaries, no buffering can be done.
	if (lba_lb != lba_ub) {
		size_t rd_blk_cnt = (n + blkdev->blk_size - 1) / blkdev->blk_size;
		uint8_t *rd_buf = kheap_alloc(blkdev->blk_size * rd_blk_cnt);
		if (blkdev_rd(blkdev, rd_buf, lba_lb, rd_blk_cnt)) {
			ku_println(LT_ERR, "buf: failed to read blocks from device (0x%x)!", blkdev);
			return 1;
		}
		
		ku_memcpy(dst, &rd_buf[byte_addr], n);
		kheap_free(rd_buf);
		return 0;
	}
	
	if (lba_lb != cache->loaded_blk) {
		if (blkdev_rd(blkdev, cache->buf, lba_lb, 1)) {
			ku_println(LT_ERR, "buf: failed to load new block buffer from device (0x%x)!", blkdev);
			return 1;
		}
		cache->loaded_blk = lba_lb;
	}
	
	ku_memcpy(dst, &cache->buf[byte_addr], n);
	return 0;
}
