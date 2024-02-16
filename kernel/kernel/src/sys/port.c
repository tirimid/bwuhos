#include "sys/port.h"

#if defined(K_ARCH_X86_64)
#include "arch/port.h"
#endif

uint64_t
port_rd(port_t p, enum port_size size)
{
#if defined(K_ARCH_X86_64)
	switch (size) {
	case PS_8:
		return port_rd_8(p & 0xffff);
	case PS_16:
		return port_rd_16(p & 0xffff);
	case PS_32:
		return port_rd_32(p & 0xffff);
	case PS_64:
		// 64 bit port read doesn't exist on x86_64.
		return 0;
	}
#endif
}

void
port_wr(port_t p, uint64_t data, enum port_size size)
{
#if defined(K_ARCH_X86_64)
	switch (size) {
	case PS_8:
		port_wr_8(p & 0xffff, data & 0xff);
		break;
	case PS_16:
		port_wr_16(p & 0xffff, data & 0xffff);
		break;
	case PS_32:
		port_wr_32(p & 0xffff, data & 0xffffffff);
		break;
	case PS_64:
		// 64 bit port write doesn't exist on x86_64.
		break;
	}
#endif
}
