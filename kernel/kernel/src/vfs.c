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
	// prevent mounting the same block device multiple times.
	// not preventing it would obviously be problematic, e.g. allowing the
	// user to mount a single drive using multiple filesystem drivers, which
	// might cause the disk data to get a little fucky wucky.
	// can check for device mount by ensuring that it has FS type null.
	if (vfs_get_mount_fs(blkdev) != VF_NULL) {
		ku_println(LT_ERR, "vfs: cannot multi-mount a single block device (0x%x)!", blkdev);
		return VFS_DRIVE_ID_NULL;
	}
	
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

enum vfs_fs
vfs_get_mount_fs(struct blkdev const *blkdev)
{
	for (size_t i = 0; i < sizeof(drives) / sizeof(drives[0]); ++i) {
		if (drives[i].flags & DF_MOUNTED && drives[i].blkdev == blkdev)
			return drives[i].fs;
	}
	
	return VF_NULL;
}

int
vfs_open(struct vfs_file *out, char const *path, uint8_t flags)
{
	// parse drive ID out of path.
	size_t drive_id_ch_cnt = 0;
	while (path[drive_id_ch_cnt] && path[drive_id_ch_cnt] != ':')
		++drive_id_ch_cnt;
	
	if (!drive_id_ch_cnt || drive_id_ch_cnt > 3) {
		ku_println(LT_ERR, "vfs: path has invalid drive ID - %s!", path);
		return 1;
	}
	
	int drive_id;
	for (size_t i = 0; i < drive_id_ch_cnt; ++i) {
		if (path[i] < '0' || path[i] > '9') {
			ku_println(LT_ERR, "vfs: path has invalid drive ID - %s!", path);
			return 1;
		}
		
		int n = path[i] - '0';
		
		// multiply by correct decimal magnitude.
		for (size_t j = 0; j < drive_id_ch_cnt - i - 1; ++j)
			n *= 10;
		
		drive_id += n;
	}
	
	if (drive_id < 1 || drive_id > sizeof(drives) / sizeof(drives[0])) {
		ku_println(LT_ERR, "vfs: path has invalid drive ID - %s!", path);
		return 1;
	}
	
	// validate ID mount status.
	if (!(drives[drive_id - 1].flags & DF_MOUNTED)) {
		ku_println(LT_ERR, "vfs: tried to open path on unmounted drive (%u)!", drive_id);
		return 1;
	}
	
	// open file.
	struct vfs_fs_driver *driver = &drives[drive_id - 1].driver;
	vfs_file_id_t fid = driver->open(driver, path + drive_id_ch_cnt + 1, flags);
	if (!fid) {
		ku_println(LT_ERR, "vfs: FS driver failed to open path - %s!", path);
		return 1;
	}

	*out = (struct vfs_file){
		.drive = drive_id,
		.file = fid,
	};
	return 0;
}

int
vfs_close(struct vfs_file *file)
{
	if (!file->drive || !file->file) {
		ku_println(LT_ERR, "vfs: cannot close null drive:FID!");
		return 1;
	}
	
	struct vfs_fs_driver *driver = &drives[file->drive - 1].driver;
	
	if (!driver->is_open(driver, file->file)) {
		ku_println(LT_ERR, "vfs: cannot close unopened FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	if (driver->close(driver, file->file)) {
		ku_println(LT_ERR, "vfs: FS driver failed to close FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	return 0;
}

int
vfs_abs_tell(struct vfs_file const *file, size_t *out_pos)
{
	if (!file->drive || !file->file) {
		ku_println(LT_ERR, "vfs: cannot abs_tell null drive:FID!");
		return 1;
	}
	
	struct vfs_fs_driver *driver = &drives[file->drive - 1].driver;
	
	if (!driver->is_open(driver, file->file)) {
		ku_println(LT_ERR, "vfs: cannot abs_tell unopened FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	if (driver->abs_tell(driver, file->file, out_pos)) {
		ku_println(LT_ERR, "vfs: FS driver failed to abs_tell FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	return 0;
}

int
vfs_seek(struct vfs_file *file, enum vfs_whence whence, long long off)
{
	if (!file->drive || !file->file) {
		ku_println(LT_ERR, "vfs: cannot seek null drive:FID!");
		return 1;
	}
	
	if (whence != VW_START && whence != VW_CUR && whence != VW_END) {
		ku_println(LT_ERR, "vfs: tried to seek with invalid whence - %u!", whence);
		return 1;
	}
	
	struct vfs_fs_driver *driver = &drives[file->drive - 1].driver;
	
	if (!driver->is_open(driver, file->file)) {
		ku_println(LT_ERR, "vfs: cannot seek unopened FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	if (driver->seek(driver, file->file, whence, off)) {
		ku_println(LT_ERR, "vfs: FS driver failed to seek FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	return 0;
}

int
vfs_rd(struct vfs_file *file, void *dst, size_t n)
{
	if (!file->drive || !file->file) {
		ku_println(LT_ERR, "vfs: cannot rd null drive:FID!");
		return 1;
	}
	
	struct vfs_fs_driver *driver = &drives[file->drive - 1].driver;
	
	if (!driver->is_open(driver, file->file)) {
		ku_println(LT_ERR, "vfs: cannot rd unopened FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	if (driver->rd(driver, file->file, dst, n)) {
		ku_println(LT_ERR, "vfs: FS driver failed to rd FID - %u:%u!", file->drive, file->file);
		return 1;
	}
	
	return 0;
}

int
vfs_wr(struct vfs_file *file, void const *src, size_t n)
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
			*out_id = i + 1;
			return &drives[i];
		}
	}
	
	return NULL;
}
