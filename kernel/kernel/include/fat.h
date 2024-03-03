#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "blkdev.h"
#include "vfs.h"

// only FAT16 with VFAT is supported.

enum fat_type {
	FT_NULL = 0,
	FT_FAT_12,
	FT_FAT_16,
	FT_FAT_32,
};

struct fat_bpb {
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

struct fat_ebpb_16 {
	uint8_t drive_num;
	uint8_t res; // this isn't windows NT, is it?
	uint8_t sig;
	uint32_t vol_id;
	char vol_label[11];
	char sys_id[8];
} __attribute__((packed));

struct fat_driver {
	struct blkdev *blkdev;
};

int fat_verify(struct blkdev *blkdev);
int fat_driver_create(struct fat_driver *out, struct blkdev *blkdev);

// VFS interface.
struct vfs_fs_driver fat_vfs_fs_driver_create(struct fat_driver *driver);
void fat_vfs_driver_destroy(void *driver_data);
vfs_file_id_t fat_vfs_open(char const *path, uint8_t flags);
void fat_vfs_close(vfs_file_id_t fid);
size_t fat_vfs_abs_tell(vfs_file_id_t fid);
int fat_vfs_seek(vfs_file_id_t fid, enum vfs_whence whence, long long off);
int fat_vfs_rd(vfs_file_id_t fid, uint8_t *dst, size_t n);
int fat_vfs_wr(vfs_file_id_t fid, uint8_t const *src, size_t n);

#endif
