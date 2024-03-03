#ifndef FAT_H
#define FAT_H

// TODO: use mutexes to protect filesystem.

#include <stdint.h>

#include "blkdev.h"
#include "vfs.h"

// only FAT16 with VFAT is supported.

enum fat_dirent_attr {
	FDA_RO = 0x1,
	FDA_HIDDEN = 0x2,
	FDA_SYS = 0x4,
	FDA_VOL_ID = 0x8,
	FDA_DIR = 0x10,
	FDA_ARCHIVE = 0x20,
	
	// long filename entry.
	FDA_LFN = FDA_RO | FDA_HIDDEN | FDA_SYS | FDA_VOL_ID,
};

enum fat_file_flag {
	FFF_OPEN = 0x1,
};

struct fat_dirent_std {
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

struct fat_dirent_lfn {
	uint8_t order;
	uint16_t ent_0[5];
	uint8_t attr;
	uint8_t type;
	uint8_t chk;
	uint16_t ent_1[6];
	uint16_t zero;
	uint16_t ent_2[2];
} __attribute__((packed));

struct fat_driver {
	struct blkdev *blkdev;
	size_t sector_cnt, root_sector_cnt, data_sector_cnt;
	size_t cluster_cnt;
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
