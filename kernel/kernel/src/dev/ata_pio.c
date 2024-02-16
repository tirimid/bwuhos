#include "dev/ata_pio.h"

#include "kutil.h"

static void sel_drive(port_t bus_io_port, port_t bus_ctl_port, uint8_t dev_num);

// set to 2 to indicate uninitialized status since the only device numbers for a
// bus are 0 and 1.
static uint8_t drive_sel_sv[0x10000] = {2};

int
ata_pio_dev_get(struct ata_pio_dev *out, port_t bus_io_port,
                port_t bus_ctl_port, uint8_t dev_num)
{
	out->bus_io_port = bus_io_port,
	out->bus_ctl_port = bus_ctl_port,
	out->dev_num = dev_num,
	out->support = 0,
	
	sel_drive(bus_io_port, bus_ctl_port, dev_num);
	
	// initial ID to determine device information.
	// since support is not yet determined, we cannot know whether to write
	// the 16 bit or 8 bit port registers.
	// so, just write both for now, and save result in support.
	port_wr(bus_io_port + APIR_SECTOR_CNT, 0, PS_16);
	port_wr(bus_io_port + APIR_SECTOR_NUM, 0, PS_16);
	port_wr(bus_io_port + APIR_CYL_LOW, 0, PS_16);
	port_wr(bus_io_port + APIR_CYL_HIGH, 0, PS_16);
	port_wr(bus_io_port + APIR_SECTOR_CNT, 0, PS_8);
	port_wr(bus_io_port + APIR_SECTOR_NUM, 0, PS_8);
	port_wr(bus_io_port + APIR_CYL_LOW, 0, PS_8);
	port_wr(bus_io_port + APIR_CYL_HIGH, 0, PS_8);
	
	ku_spin_cycles(100);
	
	port_wr(bus_io_port + APIR_CMD, 0xec, PS_8); // IDENTIFY.
	uint8_t status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	if (!status)
		return 1;
	
	// TODO: check for non-standard ATAPI and give error if found.
	while (status & APS_BSY)
		status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	
	while (!(status & APS_DRQ) && !(status & APS_ERR))
		status = port_rd(bus_io_port + APIR_STATUS, PS_8);
	
	if (status & APS_ERR)
		return 1;
	
	uint16_t id_buf[256];
	for (size_t i = 0; i < 256; ++i)
		id_buf[i] = port_rd(bus_io_port + APIR_DATA, PS_16);
	
	out->sector_size = id_buf[51];
	out->nsector = id_buf[60] | (size_t)id_buf[61] << 16;
	
	out->support |= (APDS_LBA | APDS_LBA_48) * !!(id_buf[83] & 0x400);
	out->support |= APDS_LBA * !!out->nsector;
	
	if (!(out->support & APDS_LBA)) {
		ku_println(LT_ERR, "ATA device at 0x%x (p) uses CHS which is unsupported!", bus_io_port);
		return 1;
	}
	
	return 0;
}

int
ata_pio_dev_id(struct ata_pio_dev const *dev, uint16_t *out_id)
{
	sel_drive(dev->bus_io_port, dev->bus_ctl_port, dev->dev_num);
	
	enum port_size ps = dev->support & APDS_LBA_48 ? PS_16 : PS_8;
	port_wr(dev->bus_io_port + APIR_SECTOR_CNT, 0, ps);
	port_wr(dev->bus_io_port + APIR_SECTOR_NUM, 0, ps);
	port_wr(dev->bus_io_port + APIR_CYL_LOW, 0, ps);
	port_wr(dev->bus_io_port + APIR_CYL_HIGH, 0, ps);
	
	ku_spin_cycles(100);
	
	port_wr(dev->bus_io_port + APIR_CMD, 0xec, PS_8); // IDENTIFY.
	
	uint8_t status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	// TODO: check for non-standard ATAPI and give error if found.
	while (status & APS_BSY)
		status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	while (!(status & APS_DRQ) && !(status & APS_ERR))
		status = port_rd(dev->bus_io_port + APIR_STATUS, PS_8);
	
	if (status & APS_ERR)
		return 1;
	
	for (size_t i = 0; i < 256; ++i)
		out_id[i] = port_rd(dev->bus_io_port + APIR_DATA, PS_16);
	
	return 0;
}

int
ata_pio_dev_rd(struct ata_pio_dev const *dev, void *dst, blk_addr_t src,
               size_t nsector)
{
	if (!nsector)
		return 0;
	
	size_t max_sectors = dev->support & APDS_LBA_48 ? 0x10000 : 0x100;
	if (nsector > max_sectors)
		return 1;
	
	// for convenience, `nsector` is wrapped.
	// that way you don't need to worry about adjusting for max read size
	// being at `nsector` = 0.
	nsector = nsector == max_sectors ? 0 : nsector;
	
	// TODO: finish implementing ATA PIO read.
	
	return 0;
}

int
ata_pio_dev_wr(struct ata_pio_dev const *dev, blk_addr_t dst, void const *src,
               size_t nsector)
{
	// TODO: implement ATA PIO write.
	return 1;
}

static void
sel_drive(port_t bus_io_port, port_t bus_ctl_port, uint8_t dev_num)
{
	// avoid doing unnecessary port operations by only switching when
	// actually necessary.
	if (drive_sel_sv[bus_io_port] != dev_num) {
		port_wr(bus_io_port + APIR_DRIVE_SEL, 0xa | APDF_DRV * dev_num, PS_8);
		ku_spin_cycles(1000);
		drive_sel_sv[bus_io_port] = dev_num;
	}
}
