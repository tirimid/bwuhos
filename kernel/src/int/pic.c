#include "int/pic.h"

#include "sys/port.h"

void
pic_init(void)
{
	// TODO: implement PIC remap.
	
	pic_set_mask(0xff, 0xff);
}

void
pic_get_mask(uint8_t *out_mask_1, uint8_t *out_mask_2)
{
	*out_mask_1 = port_rd_8(P_PIC1_DATA);
	*out_mask_2 = port_rd_8(P_PIC2_DATA);
}

void
pic_set_mask(uint8_t mask_1, uint8_t mask_2)
{
	port_wr_8(P_PIC1_DATA, mask_1);
	port_wr_8(P_PIC2_DATA, mask_2);
}

void
pic_eoi(uint8_t irq)
{
	if (irq >= 8)
		port_wr_8(P_PIC2_CMD, 0x20);
	port_wr_8(P_PIC1_CMD, 0x20);
}
