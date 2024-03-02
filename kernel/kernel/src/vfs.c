#include "vfs.h"

enum drive_flag {
	DF_MOUNTED = 0x1,
};

enum fs {
	F_NONE = 0,
	F_FAT_16,
};

struct drive {
	struct blkdev *blkdev;
	unsigned char fs;
	uint8_t flags;
};

// drive ID is represented as 8-bit unsigned integer.
// max drive count is limited to 255 (256 subtracting 1 to account for 0 being
// invalid).
struct drive drives[255];

vfs_drive_id_t
vfs_mount(struct blkdev *blkdev)
{
	// TODO: implement.
}

void
vfs_unmount(vfs_drive_id_t id)
{
	// TODO: implement.
}

vfs_file_handle_t
vfs_open(char const *path)
{
	// TODO: implement.
}

void
vfs_close(vfs_file_handle_t file)
{
	// TODO: implement.
}

