#ifndef FAT_H
#define FAT_H

// TODO: use mutexes to protect filesystem.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "blk_cache.h"
#include "blkdev.h"
#include "vfs.h"

#define FAT_MAX_FILE_DESCS 128

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

enum fat_file_desc_flag {
	FFDF_OPEN = 0x1,
	FFDF_READ = 0x2,
	FFDF_WRITE = 0x4,
};

struct fat_dirent_info {
	// driver will truncate UCS-2 names to normal chars.
	// this effectively means that it is unable to properly differentiate
	// between different >8-bit-char filenames.
	// TODO: implement a proper UCS-2 to char conversion.
	char name[255];
	
	uint8_t time_mk_h, time_mk_m, time_mk_s;
	uint16_t date_mk_y;
	uint8_t date_mk_m, date_mk_d;
	
	uint16_t date_acc_y;
	uint8_t date_acc_m, date_acc_d;
	
	uint8_t time_mod_h, time_mod_m, time_mod_s;
	uint16_t date_mod_y;
	uint8_t date_mod_m, date_mod_d;
	
	uint8_t attr;
	
	size_t cluster, size;
};

struct fat_file_desc {
	size_t cluster, size;
	size_t file_pos;
	uint8_t flags;
};

struct fat_driver {
	struct blk_cache blk_cache;
	
	struct fat_file_desc file_descs[FAT_MAX_FILE_DESCS];
	
	size_t sector_size, cluster_size;
	blk_addr_t first_data_sector;
	size_t sector_cnt, root_sector_cnt, data_sector_cnt;
	size_t cluster_cnt;
};

int fat_verify(struct blkdev *blkdev);
int fat_driver_create(struct fat_driver *out, struct blkdev *blkdev);
uintptr_t fat_get_root(struct fat_driver *driver);
uintptr_t fat_next_dirent(struct fat_driver *driver, uintptr_t cur_de);
int fat_get_dirent_info(struct fat_driver *driver, struct fat_dirent_info *out, uintptr_t de);
uintptr_t fat_find_dirent(struct fat_driver *driver, uintptr_t cur_de, char const *name);

// VFS interface.
struct vfs_fs_driver fat_vfs_fs_driver_create(struct fat_driver *driver);
void fat_vfs_driver_destroy(void *driver_data);
bool fat_vfs_is_open(struct vfs_fs_driver *driver, vfs_file_id_t fid);
vfs_file_id_t fat_vfs_open(struct vfs_fs_driver *driver, char const *path, uint8_t flags);
int fat_vfs_close(struct vfs_fs_driver *driver, vfs_file_id_t fid);
int fat_vfs_abs_tell(struct vfs_fs_driver *driver, vfs_file_id_t fid, size_t *out);
int fat_vfs_seek(struct vfs_fs_driver *driver, vfs_file_id_t fid, enum vfs_whence whence, long long off);
int fat_vfs_rd(struct vfs_fs_driver *driver, vfs_file_id_t fid, uint8_t *dst, size_t n);
int fat_vfs_wr(struct vfs_fs_driver *driver, vfs_file_id_t fid, uint8_t const *src, size_t n);

#endif
