#include "int/pic.h"

#include "sys/port.h"

enum icw1 {
	ICW1_ICW4 = 0x1,
	ICW1_SINGLE = 0x2,
	ICW1_LEVEL = 0x8,
	ICW1_INIT = 0x10,
};

enum icw4 {
	ICW4_8086 = 0x1,
	ICW4_AUTO = 0x2,
	ICW4_BUF_SLAVE = 0x8,
	ICW4_BUF_MASTER = 0xc,
	ICW4_SFNM = 0x10,
};

void
pic_init(void)
{
	port_wr_8(P_PIC1_CMD, ICW1_ICW4 | ICW1_INIT);
	port_wait();
	port_wr_8(P_PIC2_CMD, ICW1_ICW4 | ICW1_INIT);
	port_wait();
	
	port_wr_8(P_PIC1_DATA, 0x20);
	port_wait();
	port_wr_8(P_PIC2_DATA, 0x28);
	port_wait();
	
	port_wr_8(P_PIC1_DATA, 4);
	port_wait();
	port_wr_8(P_PIC2_DATA, 2);
	port_wait();
	
	port_wr_8(P_PIC1_DATA, ICW4_8086);
	port_wait();
	port_wr_8(P_PIC2_DATA, ICW4_8086);
	port_wait();
	
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
