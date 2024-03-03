#include "fat.h"

#include "kheap.h"
#include "kutil.h"

int
fat_verify(struct blkdev *blkdev)
{
	if (blkdev->blk_size != 512)
		return 1;
	
	uint8_t buf[512];
	if (blkdev_rd(blkdev, buf, 0, 1)) {
		ku_println(LT_ERR, "fat: failed to read BPB/EBPB!");
		return 1;
	}
	
	if (*(uint16_t *)&buf[510] != 0xaa55)
		return 1;
	
	struct fat_bpb *bpb = (struct fat_bpb *)buf;
	struct fat_ebpb_16 *ebpb = (struct fat_ebpb_16 *)&buf[sizeof(*bpb)];
	if (bpb->sector_size != 512)
		return 1;
	
	// FAT32 not supported.
	if (!bpb->fat_sectors)
		return 1;
	
	size_t sector_cnt = bpb->sector_cnt ? bpb->sector_cnt : bpb->large_sector_cnt;
	size_t root_sector_cnt = (32 * bpb->root_dirent_cnt + bpb->sector_size - 1) / bpb->sector_size;
	size_t data_sector_cnt = sector_cnt - bpb->res_sector_cnt - bpb->fat_cnt * bpb->fat_sectors - root_sector_cnt;
	size_t cluster_cnt = data_sector_cnt / bpb->cluster_size;
	
	// ExFAT not supported.
	if (!sector_cnt)
		return 1;
	
	// FAT12 not supported.
	if (cluster_cnt < 4085)
		return 1;
	
	return 0;
}

int
fat_driver_create(struct fat_driver *out, struct blkdev *blkdev)
{
	if (blkdev->blk_size != 512) {
		ku_println(LT_ERR, "fat: non-512-byte block size devices are not supported by FAT driver!");
		return 1;
	}
	
	uint8_t buf[512];
	if (blkdev_rd(blkdev, buf, 0, 1)) {
		ku_println(LT_ERR, "fat: failed to read BPB/EBPB!");
		return 1;
	}
	
	if (*(uint16_t *)&buf[510] != 0xaa55) {
		ku_println(LT_ERR, "fat: invalid boot signature!");
		return 1;
	}
	
	struct fat_bpb *bpb = (struct fat_bpb *)buf;
	struct fat_ebpb_16 *ebpb = (struct fat_ebpb_16 *)&buf[sizeof(*bpb)];
	if (bpb->sector_size != 512) {
		ku_println(LT_ERR, "fat: non-512-byte sector size FS is not supported by FAT driver!");
		return 1;
	}
	
	return 0;
}

struct vfs_fs_driver
fat_vfs_fs_driver_create(struct fat_driver *driver)
{
	struct vfs_fs_driver vfs_driver = {
		.driver_data = kheap_alloc(sizeof(*driver)),
		.driver_destroy = fat_vfs_driver_destroy,
		.open = fat_vfs_open,
		.close = fat_vfs_close,
		.abs_tell = fat_vfs_abs_tell,
		.seek = fat_vfs_seek,
		.rd = fat_vfs_rd,
		.wr = fat_vfs_wr,
	};
	*(struct fat_driver *)vfs_driver.driver_data = *driver;
	
	return vfs_driver;
}

void
fat_vfs_driver_destroy(void *driver_data)
{
	kheap_free(driver_data);
}

vfs_file_id_t
fat_vfs_open(char const *path, uint8_t flags)
{
	// TODO: implement.
}

void
fat_vfs_close(vfs_file_id_t fid)
{
	// TODO: implement.
}

size_t
fat_vfs_abs_tell(vfs_file_id_t fid)
{
	// TODO: implement.
}

int
fat_vfs_seek(vfs_file_id_t fid, enum vfs_whence whence, long long off)
{
	// TODO: implement.
}

int
fat_vfs_rd(vfs_file_id_t fid, uint8_t *dst, size_t n)
{
	// TODO: implement.
}

int
fat_vfs_wr(vfs_file_id_t fid, uint8_t const *src, size_t n)
{
	// TODO: implement.
}
