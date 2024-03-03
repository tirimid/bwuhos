#include "mbr.h"

#include "blkdev.h"
#include "kutil.h"

void
mbr_find(void)
{
	ku_println(LT_INFO, "mbr: find and partition");
	
	size_t blkdev_cnt;
	struct blkdev *blkdevs = blkdevs_get(&blkdev_cnt);
	for (size_t i = 0; i < blkdev_cnt; ++i) {
		if (blkdevs[i].dev_type != BDT_DISK_DRIVE)
			continue;
		
		// TODO: add non-512-byte blocksize block.
		if (blkdevs[i].blk_size != 512) {
			ku_println(LT_WARN, "mbr: non-512-byte block size devices are not supported yet!");
			continue;
		}
		
		struct mbr mbr;
		if (blkdev_rd(&blkdevs[i], &mbr, 0, 1)) {
			ku_println(LT_WARN, "mbr: failed to read device to check for MBR!");
			continue;
		}
		
		// not really worth checking for disk ID uniqueness right now.
		if (mbr_verify(&mbr, NULL, 0))
			continue;
		
		for (size_t j = 0; j < 4; ++j) {
			if (mbr.ents[j].type == 0x0)
				continue;
			
			if (!blkdev_mk_part(&blkdevs[i], mbr.ents[j].start_lba, mbr.ents[j].nsector))
				ku_println(LT_WARN, "mbr: failed to create block device partition!");
		}
	}
}

int
mbr_verify(struct mbr const *mbr, uint32_t const *disk_ids, size_t disk_id_cnt)
{
	if (mbr->sig != 0xaa55)
		return 1;
	
	if (disk_ids) {
		for (size_t i = 0; i < disk_id_cnt; ++i) {
			if (mbr->disk_id == disk_ids[i])
				return 1;
		}
	}
	
	return 0;
}
