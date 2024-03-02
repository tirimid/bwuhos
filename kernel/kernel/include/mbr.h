#ifndef MBR_H
#define MBR_H

#include <stddef.h>
#include <stdint.h>

struct mbr_ent {
	uint8_t drive_attr;
	uint8_t first_head, first_sector_cyl, first_cyl;
	uint8_t type;
	uint8_t last_head, last_sector_cyl, last_cyl;
	uint32_t start_lba;
	uint32_t nsector;
} __attribute__((packed));

struct mbr {
	uint8_t bootstrap[440];
	uint32_t disk_id;
	uint16_t res;
	struct mbr_ent ents[4];
	uint16_t sig;
} __attribute__((packed));

void mbr_find(void);

// `disk_ids` will be checked against the `disk_id` field of the MBR to make
// sure that it is actually unique.
// when this isn't necessary, just pass `NULL` and the ID check will be skipped.
int mbr_verify(struct mbr const *mbr, uint32_t const *disk_ids, size_t disk_id_cnt);

#endif
