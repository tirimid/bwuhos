#ifndef SYS_PORT_H__
#define SYS_PORT_H__

#include <stdint.h>

enum port {
	// PIC.
	P_PIC1_CMD = 0x20,
	P_PIC1_DATA = 0x21,
	P_PIC2_CMD = 0xa0,
	P_PIC2_DATA = 0xa1,
	
	// serial COM.
	// "less reliable" COM3-8 not included.
	P_COM1 = 0x3f8,
	P_COM2 = 0x2f8,
};

uint8_t port_rd_8(enum port p);
uint16_t port_rd_16(enum port p);
uint32_t port_rd_32(enum port p);

void port_wr_8(enum port p, uint8_t b);
void port_wr_16(enum port p, uint16_t w);
void port_wr_32(enum port p, uint32_t l);

void port_wait(void);

#endif
