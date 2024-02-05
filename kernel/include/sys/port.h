#ifndef SYS_PORT_H__
#define SYS_PORT_H__

#include <stdint.h>

enum port {
	P_PIC_MASTER_CMD = 0x20,
	P_PIC_MASTER_DATA = 0x21,
	P_PIC_SLAVE_CMD = 0xa0,
	P_PIC_SLAVE_DATA = 0xa1,
};

uint8_t port_rd_8(enum port p);
uint16_t port_rd_16(enum port p);
uint32_t port_rd_32(enum port p);

void port_wr_8(enum port p, uint8_t b);
void port_wr_16(enum port p, uint16_t w);
void port_wr_32(enum port p, uint32_t l);

#endif
