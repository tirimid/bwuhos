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

static uintptr_t cluster_to_pos(struct fat_driver const *driver, size_t cluster);
static struct fat_file_desc *find_slot(struct fat_driver *driver, vfs_file_id_t *out_id);

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
		.cluster_size = bpb->cluster_size,
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
		.is_open = fat_vfs_is_open,
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
	uint8_t rd_buf[32];
	
	// skip to end of current direct (since there may be multiple actual
	// directory entries in a VFAT-compliant entry).
	uintptr_t cur_de_last;
	for (cur_de_last = cur_de;; cur_de_last += 32) {
		if (blk_cache_rd(&driver->blk_cache, rd_buf, cur_de_last, 32)) {
			ku_println(LT_ERR, "fat: failed to read block cache!");
			return 1;
		}
		
		// directory EOF.
		if (!rd_buf[0])
			return 0;
		
		// unused entry.
		if (rd_buf[0] == 0xe5)
			continue;
		
		struct dirent_std *buf_de_std = (struct dirent_std *)rd_buf;
		if (buf_de_std->attr == FDA_LFN)
			continue;
		
		break;
	}
	
	// get next entry after end.
	uintptr_t next_de;
	for (next_de = cur_de_last + 32;; next_de += 32) {
		if (blk_cache_rd(&driver->blk_cache, rd_buf, cur_de_last, 32)) {
			ku_println(LT_ERR, "fat: failed to read block cache!");
			return 1;
		}
		
		// directory EOF.
		if (!rd_buf[0])
			return 0;
		
		if (rd_buf[0] == 0xe5)
			continue;
		
		break;
	}
	
	return next_de;
}

int
fat_get_dirent_info(struct fat_driver *driver, struct fat_dirent_info *out,
                    uintptr_t de)
{
	char name[255] = {0};
	uint8_t rd_buf[32];
	struct dirent_std *buf_de_std;
	for (;; de += 32) {
		if (blk_cache_rd(&driver->blk_cache, rd_buf, de, 32)) {
			ku_println(LT_ERR, "fat: failed to read block cache!");
			return 1;
		}
		
		// directory EOF.
		if (!rd_buf[0])
			return 1;
		
		// unused entry.
		if (rd_buf[0] == 0xe5)
			continue;
		
		buf_de_std = (struct dirent_std *)rd_buf;
		if (buf_de_std->attr == FDA_LFN) {
			struct dirent_lfn *buf_de_lfn = (struct dirent_lfn *)rd_buf;
			
			// copy name segment.
			for (size_t i = 0; i < 5; ++i)
				name[13 * buf_de_lfn->order + i] = buf_de_lfn->ent_0[i];
			for (size_t i = 0; i < 6; ++i)
				name[13 * buf_de_lfn->order + 5 + i] = buf_de_lfn->ent_1[i];
			for (size_t i = 0; i < 2; ++i)
				name[13 * buf_de_lfn->order + 11 + i] = buf_de_lfn->ent_2[i];
			
			continue;
		}
		
		break;
	}
	
	if (!name[0]) {
		// convert DOS extension format to something sane.
		size_t name_len;
		for (name_len = 0; name_len < 8; ++name_len) {
			if (buf_de_std->name[name_len] == ' ')
				break;
		}
		
		size_t ext_len;
		for (ext_len = 0; ext_len < 3; ++ext_len) {
			if (buf_de_std->name[8 + ext_len] == ' ')
				break;
		}
		
		if (name_len && ext_len) {
			ku_memcpy(name, buf_de_std->name, name_len);
			ku_memcpy(&name[name_len], ".", 1);
			ku_memcpy(&name[name_len + 1], &buf_de_std->name[8], ext_len);
		} else if (name_len)
			ku_memcpy(name, buf_de_std->name, name_len);
		else if (ext_len)
			ku_memcpy(name, &buf_de_std->name[8], ext_len);
	}
	
	*out = (struct fat_dirent_info){
		.time_mk_h = buf_de_std->time_mk_hms & 0x1f,
		.time_mk_m = buf_de_std->time_mk_hms >> 5 & 0x3f,
		.time_mk_s = 2 * (buf_de_std->time_mk_hms >> 11 & 0x1f),
		.date_mk_y = buf_de_std->time_mk_date & 0x7f,
		.date_mk_m = buf_de_std->time_mk_date >> 7 & 0xf,
		.date_mk_d = buf_de_std->time_mk_date >> 11 & 0x1f,
		
		.date_acc_y = buf_de_std->time_acc_date & 0x7f,
		.date_acc_m = buf_de_std->time_acc_date >> 7 & 0xf,
		.date_acc_d = buf_de_std->time_acc_date >> 11 & 0x1f,
		
		.time_mod_h = buf_de_std->time_mod_hms & 0x1f,
		.time_mod_m = buf_de_std->time_mod_hms >> 5 & 0x3f,
		.time_mod_s = 2 * (buf_de_std->time_mod_hms >> 11 & 0x1f),
		.date_mod_y = buf_de_std->time_mod_date & 0x7f,
		.date_mod_m = buf_de_std->time_mod_date >> 7 & 0xf,
		.date_mod_d = buf_de_std->time_mod_date >> 11 & 0x1f,
		
		.attr = buf_de_std->attr,
		
		.cluster = buf_de_std->first_cluster_0,
		.size = buf_de_std->file_size,
	};
	ku_memcpy(out->name, name, sizeof(out->name));
	return 0;
}

uintptr_t
fat_find_dirent(struct fat_driver *driver, uintptr_t cur_de, char const *name)
{
	uintptr_t de = cur_de;
	do {
		struct fat_dirent_info de_info;
		if (fat_get_dirent_info(driver, &de_info, de)) {
			ku_println(LT_ERR, "fat: failed to get info of dirent at 0x%x!", de);
			return 0;
		}
		
		int diff = 0;
		for (size_t i = 0; name[i] && de_info.name[i]; ++i) {
			char a_lower = name[i];
			if (a_lower >= 'A' && a_lower <= 'Z')
				a_lower = a_lower - 'A' + 'a';
			
			char b_lower = de_info.name[i];
			if (b_lower >= 'A' && b_lower <= 'Z')
				b_lower = b_lower - 'A' + 'a';
			
			if (a_lower > b_lower) {
				diff = 1;
				break;
			} else if (a_lower < b_lower) {
				diff = -1;
				break;
			}
		}
		
		if (!diff)
			return de;
	} while (de = fat_next_dirent(driver, de));
	
	return 0;
}

void
fat_vfs_driver_destroy(void *driver_data)
{
	kheap_free(driver_data);
}

bool
fat_vfs_is_open(struct vfs_fs_driver *driver, vfs_file_id_t fid)
{
	struct fat_driver *dv = driver->driver_data;
	return !!(dv->file_descs[fid - 1].flags & FFDF_OPEN);
}

vfs_file_id_t
fat_vfs_open(struct vfs_fs_driver *driver, char const *path, uint8_t flags)
{
	struct fat_driver *dv = driver->driver_data;
	
	char path_buf[256];
	size_t path_len = 0;
	uintptr_t de = fat_get_root(dv);
	struct fat_dirent_info de_info;
	
	for (size_t i = 0; path[i]; ++i) {
		if (path[i] != '/') {
			path_buf[path_len++] = path[i];
			continue;
		}
		
		if (!path_len)
			continue;
		
		path_buf[path_len] = 0;
		path_len = 0;
		if (!(de = fat_find_dirent(dv, de, path_buf)))
			return VFS_FILE_ID_NULL;
		
		if (fat_get_dirent_info(dv, &de_info, de)) {
			ku_println(LT_ERR, "fat: failed to get info of dirent at 0x%x!", de);
			return VFS_FILE_ID_NULL;
		}
		
		if (!(de_info.attr & FDA_DIR) && path[i + 1]) {
			ku_println(LT_ERR, "fat: file dirent (0x%x) treated as directory!", de);
			return VFS_FILE_ID_NULL;
		}
		
		de = cluster_to_pos(dv, de_info.cluster);
	}
	
	// if last char is not '/', the last dirent level would not be found.
	// rectify this.
	if (path_len) {
		path_buf[path_len] = 0;
		if (!(de = fat_find_dirent(dv, de, path_buf)))
			return VFS_FILE_ID_NULL;
		
		if (fat_get_dirent_info(dv, &de_info, de)) {
			ku_println(LT_ERR, "fat: failed to get info of dirent at 0x%x!", de);
			return VFS_FILE_ID_NULL;
		}
		
		de = cluster_to_pos(dv, de_info.cluster);
	}
	
	if (de_info.attr & FDA_DIR) {
		ku_println(LT_ERR, "fat: cannot open path as file - %s!", path);
		return VFS_FILE_ID_NULL;
	}
	
	if (flags & VFF_WRITE && de_info.attr & FDA_RO) {
		ku_println(LT_ERR, "fat: cannot open read-only file with write - %s!", path);
		return VFS_FILE_ID_NULL;
	}
	
	vfs_file_id_t fid;
	struct fat_file_desc *fd = find_slot(dv, &fid);
	if (!fd) {
		ku_println(LT_ERR, "fat: could not find any free slots for file - %s!", path);
		return VFS_FILE_ID_NULL;
	}
	
	*fd = (struct fat_file_desc){
		.cluster = de_info.cluster,
		.size = de_info.size,
		.file_pos = 0,
		.flags = FFDF_OPEN | FFDF_READ * !!(flags & VFF_READ) | FFDF_WRITE * !!(flags & VFF_WRITE),
	};
	
	return fid;
}

int
fat_vfs_close(struct vfs_fs_driver *driver, vfs_file_id_t fid)
{
	struct fat_driver *dv = driver->driver_data;
	
	dv->file_descs[fid - 1] = (struct fat_file_desc){
		.cluster = 0,
		.size = 0,
		.file_pos = 0,
		.flags = 0,
	};
	
	return 0;
}

int
fat_vfs_abs_tell(struct vfs_fs_driver *driver, vfs_file_id_t fid, size_t *out)
{
	struct fat_driver *dv = driver->driver_data;
	*out = dv->file_descs[fid - 1].file_pos;
	return 0;
}

int
fat_vfs_seek(struct vfs_fs_driver *driver, vfs_file_id_t fid,
             enum vfs_whence whence, long long off)
{
	struct fat_driver *dv = driver->driver_data;
	struct fat_file_desc *fd = &dv->file_descs[fid - 1];
	
	size_t clamp_ub = fd->size > 0 ? fd->size - 1 : 0;
	
	switch (whence) {
	case VW_SET:
		off = off < 0 ? 0 : off;
		off = off > clamp_ub ? clamp_ub : off;
		fd->file_pos = off;
		break;
	case VW_CUR:
		off = off < -(long long)fd->file_pos ? -(long long)fd->file_pos : off;
		off = off > clamp_ub - fd->file_pos ? clamp_ub - fd->file_pos : off;
		fd->file_pos += off;
		break;
	case VW_END:
		off = off < -(long long)clamp_ub ? -(long long)clamp_ub : off;
		off = off > 0 ? 0 : off;
		fd->file_pos = clamp_ub + off;
		break;
	}
	
	return 0;
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

static uintptr_t
cluster_to_pos(struct fat_driver const *driver, size_t cluster)
{
	size_t sector_off = (cluster - 2) * driver->cluster_size;
	return driver->sector_size * (driver->first_data_sector + sector_off);
}

static struct fat_file_desc *
find_slot(struct fat_driver *driver, vfs_file_id_t *out_id)
{
	for (size_t i = 0; i < FAT_MAX_FILE_DESCS; ++i) {
		if (!(driver->file_descs[i].flags & FFDF_OPEN)) {
			*out_id = i + 1;
			return &driver->file_descs[i];
		}
	}
	
	return NULL;
}
