#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#include "blkdev.h"

#define VFS_DRIVE_ID_NULL 0
#define VFS_FILE_HANDLE_NULL 0

// 0 reserved as failure value.
// for this reason, drive numbers begin at 1 and drive ids are always positive.
typedef uint8_t vfs_drive_id_t;

// 0 as failure value.
typedef size_t vfs_file_handle_t;

vfs_drive_id_t vfs_mount(struct blkdev *blkdev);
void vfs_unmount(vfs_drive_id_t id);
vfs_file_handle_t vfs_open(char const *path);
void vfs_close(vfs_file_handle_t file);

#endif
