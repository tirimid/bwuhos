#ifndef ARCH_PORT_H
#define ARCH_PORT_H

#include <stdint.h>

uint8_t port_rd_8(uint16_t src);
uint16_t port_rd_16(uint16_t src);
uint32_t port_rd_32(uint16_t src);

void port_wr_8(uint16_t dst, uint8_t b);
void port_wr_16(uint16_t dst, uint16_t w);
void port_wr_32(uint16_t dst, uint32_t l);

#endif
