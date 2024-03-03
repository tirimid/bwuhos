#include "fat.h"

#include "kheap.h"
#include "kutil.h"

struct bpb {
	uint8_t jmp[3];
	char oem_id[8];
	uint16_t sector_size; // in bytes.
	uint8_t cluster_size; // in sectors.
	uint16_t res_sector_cnt;
	uint8_t fat_cnt;
	uint16_t root_dirent_cnt;
	uint16_t sector_cnt;
	uint8_t media_desc;
	uint16_t fat_sectors;
	uint16_t track_sectors;
	uint16_t head_cnt;
	uint32_t hidden_sector_cnt;
	uint32_t large_sector_cnt;
} __attribute__((packed));

struct dirent_std {
	char name[11];
	uint8_t attr;
	uint8_t res;
	uint8_t time_mk_s100; // 100ths of a second.
	uint16_t time_mk_hms, time_mk_date;
	uint16_t time_acc_date;
	uint16_t first_cluster_1;
	uint16_t time_mod_hms, time_mod_date;
	uint16_t first_cluster_0;
	uint32_t file_size;
} __attribute__((packed));

struct dirent_lfn {
	uint8_t order;
	uint16_t ent_0[5];
	uint8_t attr;
	uint8_t type;
	uint8_t chk;
	uint16_t ent_1[6];
	uint16_t zero;
	uint16_t ent_2[2];
} __attribute__((packed));

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
	
	struct bpb *bpb = (struct bpb *)buf;
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
	if (fat_verify(blkdev)) {
		ku_println(LT_ERR, "fat: filesystem failed verification!");
		return 1;
	}
	
	uint8_t buf[512];
	if (blkdev_rd(blkdev, buf, 0, 1)) {
		ku_println(LT_ERR, "fat: failed to read BPB/EBPB!");
		return 1;
	}
	
	struct bpb *bpb = (struct bpb *)buf;
	
	size_t sector_cnt = bpb->sector_cnt ? bpb->sector_cnt : bpb->large_sector_cnt;
	size_t root_sector_cnt = (32 * bpb->root_dirent_cnt + bpb->sector_size - 1) / bpb->sector_size;
	size_t data_sector_cnt = sector_cnt - bpb->res_sector_cnt - bpb->fat_cnt * bpb->fat_sectors - root_sector_cnt;
	size_t cluster_cnt = data_sector_cnt / bpb->cluster_size;
	size_t first_data_sector = bpb->res_sector_cnt + bpb->fat_cnt * bpb->fat_sectors + root_sector_cnt;
	
	*out = (struct fat_driver){
		.blk_cache = blk_cache_create(blkdev),
		.sector_size = bpb->sector_size,
		.first_data_sector = first_data_sector,
		.sector_cnt = sector_cnt,
		.root_sector_cnt = root_sector_cnt,
		.data_sector_cnt = data_sector_cnt,
		.cluster_cnt = cluster_cnt,
	};
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

uintptr_t
fat_get_root(struct fat_driver *driver)
{
	return driver->sector_size * (driver->first_data_sector - driver->root_sector_cnt);
}

uintptr_t
fat_next_dirent(struct fat_driver *driver, uintptr_t cur_de)
{
	// TODO: implement.
}

int
fat_get_dirent_info(struct fat_driver *driver, struct fat_dirent_info *out,
                    uintptr_t de)
{
	// TODO: implement.
}

uintptr_t
fat_find_dirent(struct fat_driver *driver, uintptr_t cur_de, char const *path)
{
	// TODO: implement.
}

int
fat_read_file(struct fat_driver *driver, void *dst, uintptr_t de,
              uintptr_t file_pos, size_t n)
{
	// TODO: implement.
}

void
fat_vfs_driver_destroy(void *driver_data)
{
	kheap_free(driver_data);
}

vfs_file_id_t
fat_vfs_open(struct vfs_fs_driver *driver, char const *path, uint8_t flags)
{
	// TODO: implement.
	
	return VFS_FILE_ID_NULL;
}

void
fat_vfs_close(struct vfs_fs_driver *driver, vfs_file_id_t fid)
{
	// TODO: implement.
}

size_t
fat_vfs_abs_tell(struct vfs_fs_driver *driver, vfs_file_id_t fid)
{
	// TODO: implement.
}

int
fat_vfs_seek(struct vfs_fs_driver *driver, vfs_file_id_t fid,
             enum vfs_whence whence, long long off)
{
	// TODO: implement.
}

int
fat_vfs_rd(struct vfs_fs_driver *driver, vfs_file_id_t fid, uint8_t *dst,
           size_t n)
{
	// TODO: implement.
}

int
fat_vfs_wr(struct vfs_fs_driver *driver, vfs_file_id_t fid, uint8_t const *src,
           size_t n)
{
	// TODO: implement.
}
