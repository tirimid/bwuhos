#include "vfs.h"

#include <stddef.h>

#include "fat.h"
#include "kutil.h"

enum drive_flag {
	DF_MOUNTED = 0x1,
};

struct drive {
	struct vfs_fs_driver driver;
	struct blkdev *blkdev;
	unsigned char fs;
	uint8_t flags;
};

// drive ID is represented as 8-bit unsigned integer.
// max drive count is limited to 255 (256 subtracting 1 to account for 0 being
// invalid).
struct drive drives[255] = {
	{
		.blkdev = NULL,
		.fs = VF_NULL,
		.flags = 0,
	},
};

static void try_mount_tree(struct blkdev *blkdev);
static struct drive *find_slot(vfs_drive_id_t *out_id);

void
vfs_auto_mount(void)
{
	ku_println(LT_INFO, "vfs: auto-mounting all drives");
	
	size_t blkdev_cnt;
	struct blkdev *blkdevs = blkdevs_get(&blkdev_cnt);
	for (size_t i = 0; i < blkdev_cnt; ++i)
		try_mount_tree(&blkdevs[i]);
}

void
vfs_unmount_all(void)
{
	// TODO: implement.
}

size_t
vfs_cnt_mounted(void)
{
	size_t nmounted = 0;
	for (size_t i = 0; i < sizeof(drives) / sizeof(drives[0]); ++i)
		nmounted += !!(drives[i].flags & DF_MOUNTED);
	return nmounted;
}

vfs_drive_id_t
vfs_mount(struct blkdev *blkdev, enum vfs_fs fs)
{
	vfs_drive_id_t slot_id;
	struct drive *slot = find_slot(&slot_id);
	if (!slot) {
		ku_println(LT_ERR, "vfs: block device (0x%x) could not find a slot to mount!", blkdev);
		return VFS_DRIVE_ID_NULL;
	}
	
	struct vfs_fs_driver vfs_driver;
	switch (fs) {
	case VF_FAT_16: {
		struct fat_driver fs_driver;
		if (fat_driver_create(&fs_driver, blkdev))
			return VFS_DRIVE_ID_NULL;
		vfs_driver = fat_vfs_fs_driver_create(&fs_driver);
		break;
	}
	default:
		ku_println(LT_ERR, "vfs: block device (0x%x) tried mounting with invalid fs type (%u)!", blkdev, fs);
		return VFS_DRIVE_ID_NULL;
	}
	
	*slot = (struct drive){
		.driver = vfs_driver,
		.blkdev = blkdev,
		.fs = fs,
		.flags = DF_MOUNTED,
	};
	
	ku_println(LT_INFO, "vfs: block device (0x%x) mounted on slot %u", blkdev, slot_id);
	return slot_id;
}

void
vfs_unmount(vfs_drive_id_t id)
{
	// TODO: implement.
}

struct vfs_file
vfs_open(char const *path, uint8_t flags)
{
	// TODO: implement.
}

void
vfs_close(struct vfs_file *file)
{
	// TODO: implement.
}

size_t
vfs_abs_tell(struct vfs_file const *file)
{
	// TODO: implement.
}

int
vfs_seek(struct vfs_file *file, enum vfs_whence whence, long long off)
{
	// TODO: implement.
}

int
vfs_rd(struct vfs_file *file, uint8_t *dst, size_t n)
{
	// TODO: implement.
}

int
vfs_wr(struct vfs_file *file, uint8_t const *src, size_t n)
{
	// TODO: implement.
}

static void
try_mount_tree(struct blkdev *blkdev)
{
	if (blkdev->dev_type == BDT_DISK_DRIVE) {
		// probe filesystem and mount appropriate VFS driver.
		if (!fat_verify(blkdev))
			vfs_mount(blkdev, VF_FAT_16);
	}
	
	for (size_t i = 0; i < blkdev->nchildren; ++i)
		try_mount_tree(&blkdev->children[i]);
}

static struct drive *
find_slot(vfs_drive_id_t *out_id)
{
	for (size_t i = 0; i < sizeof(drives) / sizeof(drives[0]); ++i) {
		if (!(drives[i].flags & DF_MOUNTED)) {
			*out_id = i;
			return &drives[i];
		}
	}
	
	return NULL;
}
