#ifndef SYS_PORT_H__
#define SYS_PORT_H__

#include <stdint.h>

#if defined(K_ARCH_X86_64)
typedef uint16_t port_t;
#endif

enum port_size {
	PS_8 = 0,
	PS_16,
	PS_32,
	PS_64,
};

uint64_t port_rd(port_t p, enum port_size size);
void port_wr(port_t p, uint64_t data, enum port_size size);

#endif
