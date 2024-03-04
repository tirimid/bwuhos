#include "blkdev.h"

#include "ata_pio.h"
#include "kheap.h"
#include "kutil.h"
#include "port.h"

#define MAX_BLKDEV_CNT 512

static char const *blkdev_info_str(struct blkdev const *blkdev);

static struct blkdev blkdevs[MAX_BLKDEV_CNT] = {
	{
		.driver_data = NULL,
		.driver_destroy = NULL,
		.rd = NULL,
		.wr = NULL,
		.children = NULL,
		.nchildren = 0,
		.part_base = 0,
		.flags = 0,
		.dev_type = BDT_NULL,
		.driver = BD_NULL,
		.blk_size = 0,
		.nblk = 0,
		.mutex = 0,
		.parent_mutex = NULL,
	},
};
static size_t blkdev_cnt;

void
blkdevs_find(void)
{
	ku_println(LT_INFO, "blkdev: finding devices");
	
	blkdev_cnt = 0;
	
	// probe likely ATA PIO ports.
	// this will probably output errors, these can be ignored.
	static port_t const ata_pio_io[] = {0x1f0, 0x170};
	static port_t const ata_pio_ctl[] = {0x3f6, 0x376};
	for (size_t i = 0; i < sizeof(ata_pio_io) / sizeof(port_t); ++i) {
		for (int j = 0; j < 2; ++j) {
			struct ata_pio_dev dev;
			if (ata_pio_dev_get(&dev, ata_pio_io[i], ata_pio_ctl[i], j))
				continue;
			
			struct blkdev blkdev = ata_pio_blkdev_create(&dev);
			if (!blkdev_register(&blkdev))
				return;
		}
	}
}

struct blkdev *
blkdev_register(struct blkdev *blkdev)
{
	if (blkdev_cnt >= MAX_BLKDEV_CNT) {
		ku_println(LT_WARN, "blkdev: unable to register %s", blkdev_info_str(blkdev));
		return NULL;
	}
	
	blkdevs[blkdev_cnt++] = *blkdev;
	ku_println(LT_INFO, "blkdev: registered %s (0x%x)", blkdev_info_str(blkdev), &blkdevs[blkdev_cnt - 1]);
	
	return &blkdevs[blkdev_cnt - 1];
}

void
blkdev_unregister(struct blkdev *blkdev)
{
	// TODO: implement.
}

struct blkdev *
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

struct blkdev
blkdev_create(void *driver_data, void (*driver_destroy)(void *),
              int (*rd)(struct blkdev *, void *, blk_addr_t, size_t),
              int (*wr)(struct blkdev *, blk_addr_t, void const *, size_t),
              enum blkdev_dev_type type, enum blkdev_driver driver,
              uint8_t flags, size_t blk_size, size_t nblk)
{
	return (struct blkdev){
		.driver_data = driver_data,
		.driver_destroy = driver_destroy,
		.rd = rd,
		.wr = wr,
		.children = kheap_alloc(sizeof(struct blkdev) * BLKDEV_MAX_PARTS),
		.nchildren = 0,
		.part_base = 0,
		.flags = flags,
		.dev_type = type,
		.driver = driver,
		.blk_size = blk_size,
		.nblk = nblk,
		.mutex = 0,
		.parent_mutex = NULL,
	};
}

struct blkdev *
blkdev_mk_part(struct blkdev *blkdev, blk_addr_t base, size_t limit)
{
	if (blkdev->flags & BF_PART) {
		ku_println(LT_ERR, "blkdev: cannot create sub-partition of partition (0x%x)!", blkdev);
		return NULL;
	}
	
	k_mutex_t *mutex = &blkdev->mutex;
	if (blkdev->flags & BF_SYNC_DEP_CHILD)
		mutex = blkdev->parent_mutex;
	
	k_mutex_lock(mutex);
	
	if (blkdev->nchildren >= BLKDEV_MAX_PARTS) {
		ku_println(LT_ERR, "blkdev: block device (0x%x) already has max partitions!", blkdev);
		k_mutex_unlock(mutex);
		return NULL;
	}
	
	if (base + limit > blkdev->nblk) {
		ku_println(LT_ERR, "blkdev: base + limit of partition would exceed block device (0x%x) - 0x%x>0x%x!", blkdev, base + limit, blkdev->nblk);
		k_mutex_unlock(mutex);
		return NULL;
	}
	
	struct blkdev part = {
		.driver_data = blkdev->driver_data,
		.driver_destroy = blkdev->driver_destroy,
		.rd = blkdev->rd,
		.wr = blkdev->wr,
		.children = NULL,
		.nchildren = 0,
		.part_base = base,
		.flags = blkdev->flags | BF_PART,
		.dev_type = blkdev->dev_type,
		.driver = blkdev->driver,
		.blk_size = blkdev->blk_size,
		.nblk = limit,
		.parent_mutex = mutex,
	};
	
	++blkdev->nchildren;
	blkdev->children[blkdev->nchildren - 1] = part;
	ku_println(LT_INFO, "blkdev: partitioned block device (0x%x) -> {0x%x..+0x%x}", blkdev, part.part_base, part.nblk);
	k_mutex_unlock(mutex);
	return &blkdev->children[blkdev->nchildren - 1];
}

void
blkdev_rm_part(struct blkdev *blkdev, size_t which)
{
	// TODO: implement.
}

size_t
blkdev_get_which(struct blkdev const *blkdev)
{
	size_t which = 0;
	for (size_t i = 0; i < blkdev_cnt; ++i) {
		if (&blkdevs[i] == blkdev)
			return which;
		
		if (blkdevs[i].dev_type == blkdev->dev_type
		    && blkdevs[i].driver == blkdev->driver) {
			++which;
		}
	}
	
	return MAX_BLKDEV_CNT;
}

void
blkdev_destroy(struct blkdev *blkdev)
{
	// TODO: implement.
}

int
blkdev_rd(struct blkdev *blkdev, void *dst, blk_addr_t src, size_t n)
{
	k_mutex_t *mutex = &blkdev->mutex;
	if (blkdev->flags & BF_PART || blkdev->flags & BF_SYNC_DEP_CHILD)
		mutex = blkdev->parent_mutex;
	
	k_mutex_lock(mutex);
	
	if (src + n > blkdev->nblk) {
		ku_println(LT_ERR, "blkdev: base + limit of read would exceed block device (0x%x) - 0x%x>0x%x!", blkdev, src + n, blkdev->nblk);
		k_mutex_unlock(mutex);
		return 1;
	}
	
	if (blkdev->flags & BF_PART)
		blkdev->rd(blkdev, dst, src + blkdev->part_base, n);
	else
		blkdev->rd(blkdev, dst, src, n);
	
	k_mutex_unlock(mutex);
	return 0;
}

int
blkdev_wr(struct blkdev *blkdev, blk_addr_t dst, void const *src, size_t n)
{
	k_mutex_t *mutex = &blkdev->mutex;
	if (blkdev->flags & BF_PART || blkdev->flags & BF_SYNC_DEP_CHILD)
		mutex = blkdev->parent_mutex;
	
	k_mutex_lock(mutex);
	
	if (dst + n > blkdev->nblk) {
		ku_println(LT_ERR, "blkdev: base + limit of write would exceed block device (0x%x) - 0x%x>0x%x!", blkdev, dst + n, blkdev->nblk);
		k_mutex_unlock(mutex);
		return 1;
	}
	
	if (blkdev->flags & BF_PART)
		blkdev->wr(blkdev, dst + blkdev->part_base, src, n);
	else
		blkdev->wr(blkdev, dst, src, n);
	
	k_mutex_unlock(mutex);
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
