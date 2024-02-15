#ifndef DEV_SATA_FIS_H__
#define DEV_SATA_FIS_H__

// datastructures implemented according to serial ATA 2.6.
// do not use the datastructures directly, only use them through pointer
// indirection to access existing memory in an organized manner.

enum sata_fis_type {
	SFT_REG_H2D = 0x27,
	SFT_REG_D2H = 0x34,
	SFT_DMA_ACTIVATE_D2H = 0x39,
	SFT_DMA_SETUP_BIDIR = 0x41,
	SFT_DATA_BIDIR = 0x46,
	SFT_BIST_ACTIVATE_BIDIR = 0x58,
	SFT_PIO_SETUP_D2H = 0x5f,
	SFT_PIO_SET_DEV_BITS_D2H = 0xa1,
	
	// reserved / vendor-specific fields according to SATA 2.6 not included.
};

struct sata_fis_reg_h2d {
	uint8_t type, pm_port_c, cmd, feats;
	uint8_t lba_low, lba_mid, lba_high, dev;
	uint8_t lba_low_exp, lba_mid_exp, lba_high_exp, dev_exp;
	uint8_t nsector, nsector_exp, _res_0, ctl;
	uint8_t _res_1[4];
} __attribute__((packed));

struct sata_fis_reg_d2h {
	uint8_t type, pm_port_int, status, err;
	uint8_t lba_low, lba_mid, lba_high, dev;
	uint8_t lba_low_exp, lba_mid_exp, lba_high_exp, _res_0;
	uint8_t _res_1[8];
} __attribute__((packed));

struct sata_fis_pio_set_dev_bits_d2h {
	uint8_t type, pm_port_int_notif, status, err;
	uint8_t _res[4];
} __attribute__((packed));

struct sata_fis_dma_activate_d2h {
	uint8_t type, pm_port, _res[2];
} __attribute__((packed));

struct sata_fis_dma_setup_bidir {
	uint8_t type, pm_port_dir_int_auto, _res_0[2];
	uint32_t dma_buf_ident_low;
	uint32_t dma_buf_ident_high;
	uint32_t _res_1;
	uint32_t dma_buf_off;
	uint32_t dma_xfer_cnt;
	uint32_t _res_2;
} __attribute__((packed));

struct sata_fis_bist_activate_bidir {
	uint8_t type, pm_port, pat_def, _res;
	uint32_t data_1;
	uint32_t data_2;
} __attribute__((packed));

struct sata_fis_pio_setup_d2h {
	uint8_t type, pm_port_dir_int, status, err;
	uint8_t lba_low, lba_mid, lba_high, dev;
	uint8_t lba_low_exp, lba_mid_exp, lba_high_exp, _res_0;
	uint8_t nsector, nsector_exp, _res_1, e_status;
	uint16_t xfer_cnt, _res_2;
} __attribute__((packed));

struct sata_fis_data_bidir_hdr {
	uint8_t type, pm_port, _res[2];
	uint32_t data[2048];
} __attribute__((packed));

#endif
