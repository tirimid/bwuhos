#include "mbr.h"

#include "kutil.h"

void
mbr_find(void)
{
	ku_println(LT_INFO, "mbr: find and register");
	
	// TODO: implement.
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
