#include "arch/pic.h"

#include "arch/autil.h"
#include "arch/port.h"

#define PORT_PIC1_CMD 0x20
#define PORT_PIC1_DATA 0x21
#define PORT_PIC2_CMD 0xa0
#define PORT_PIC2_DATA 0xa1
#define PORT_IGNORE 0x80

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
	au_println(LT_INFO, "pic: init");
	
	// remap PIC.
	port_wr_8(PORT_PIC1_CMD, ICW1_ICW4 | ICW1_INIT);
	port_wr_8(PORT_IGNORE, 0);
	port_wr_8(PORT_PIC2_CMD, ICW1_ICW4 | ICW1_INIT);
	port_wr_8(PORT_IGNORE, 0);
	
	port_wr_8(PORT_PIC1_DATA, 0x20);
	port_wr_8(PORT_IGNORE, 0);
	port_wr_8(PORT_PIC2_DATA, 0x28);
	port_wr_8(PORT_IGNORE, 0);
	
	port_wr_8(PORT_PIC1_DATA, 4);
	port_wr_8(PORT_IGNORE, 0);
	port_wr_8(PORT_PIC2_DATA, 2);
	port_wr_8(PORT_IGNORE, 0);
	
	port_wr_8(PORT_PIC1_DATA, ICW4_8086);
	port_wr_8(PORT_IGNORE, 0);
	port_wr_8(PORT_PIC2_DATA, ICW4_8086);
	port_wr_8(PORT_IGNORE, 0);
	
	// mask all interrupts.
	port_wr_8(PORT_PIC1_DATA, 0xff);
	port_wr_8(PORT_PIC2_DATA, 0xff);
}
