#ifndef VFS_H
#define VFS_H

#include <stdbool.h>
#include <stdint.h>

#include "blkdev.h"

// TODO: use mutexes to protect VFS.

#define VFS_DRIVE_ID_NULL 0
#define VFS_FILE_ID_NULL 0

// 0 reserved as failure value.
// for this reason, drive numbers begin at 1 and drive ids are always positive.
typedef uint8_t vfs_drive_id_t;

// 0 reserved as failure value.
typedef size_t vfs_file_id_t;

enum vfs_fs {
	VF_NULL = 0,
	VF_FAT_16,
};

enum vfs_whence {
	VW_SET,
	VW_CUR,
	VW_END,
};

enum vfs_file_flag {
	VFF_READ = 0x1,
	VFF_WRITE = 0x2,
};

struct vfs_file {
	vfs_drive_id_t drive;
	vfs_file_id_t file;
};

struct vfs_fs_driver {
	void *driver_data;
	
	void (*driver_destroy)(void *);
	
	// VFS internal requirements.
	bool (*is_open)(struct vfs_fs_driver *, vfs_file_id_t);
	
	// file calls.
	vfs_file_id_t (*open)(struct vfs_fs_driver *, char const *path, uint8_t);
	int (*close)(struct vfs_fs_driver *, vfs_file_id_t);
	int (*abs_tell)(struct vfs_fs_driver *, vfs_file_id_t, size_t *);
	int (*seek)(struct vfs_fs_driver *, vfs_file_id_t, enum vfs_whence, long long);
	int (*rd)(struct vfs_fs_driver *, vfs_file_id_t, uint8_t *dst, size_t n);
	int (*wr)(struct vfs_fs_driver *, vfs_file_id_t, uint8_t const *src, size_t n);
};

void vfs_auto_mount(void);
void vfs_unmount_all(void);
size_t vfs_cnt_mounted(void);
vfs_drive_id_t vfs_mount(struct blkdev *blkdev, enum vfs_fs fs);
void vfs_unmount(vfs_drive_id_t id);
enum vfs_fs vfs_get_mount_fs(struct blkdev const *blkdev);

int vfs_open(struct vfs_file *out, char const *path, uint8_t flags);
int vfs_close(struct vfs_file *file);
int vfs_abs_tell(struct vfs_file const *file, size_t *out_pos);
int vfs_seek(struct vfs_file *file, enum vfs_whence whence, long long off);
int vfs_rd(struct vfs_file *file, uint8_t *dst, size_t n);
int vfs_wr(struct vfs_file *file, uint8_t const *src, size_t n);

#endif
