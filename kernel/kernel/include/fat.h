#ifndef FAT_H
#define FAT_H

#include <stdint.h>

struct fat_bpb {
	uint8_t jmp[3];
	char oem_id[8];
	uint16_t sector_size; // in bytes.
	uint8_t cluster_size; // in sectors.
	uint16_t reserved_sector_cnt;
	uint8_t fat_cnt;
	uint16_t root_dirent_cnt;
	uint16_t lvol_sector_cnt;
	uint8_t media_desc;
	uint16_t fat_sectors;
	uint16_t track_sectors;
	uint16_t head_cnt;
	uint32_t hidden_sector_cnt;
	uint32_t large_sector_cnt;
} __attribute__((packed));

struct fat_ebpb_16 {
	uint8_t drive_num;
	uint8_t res; // this isn't windows NT, is it?
	uint8_t sig;
	uint32_t vol_id;
	char vol_label[11];
	char sys_id[8];
} __attribute__((packed));

#endif
