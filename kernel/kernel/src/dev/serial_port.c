#include "dev/serial_port.h"

#include "kdef.h"
#include "sys/port.h"

#define PORT_COM1 0x3f8

int
sp_init(void)
{
	// check OSDev wiki article "Serial Ports" for init explanation.
	port_wr(PORT_COM1 + 1, 0x0, PS_8);
	port_wr(PORT_COM1 + 3, 0x80, PS_8);
	port_wr(PORT_COM1, 0x3, PS_8);
	port_wr(PORT_COM1 + 1, 0x0, PS_8);
	port_wr(PORT_COM1 + 3, 0x3, PS_8);
	port_wr(PORT_COM1 + 2, 0xc7, PS_8);
	port_wr(PORT_COM1 + 4, 0xb, PS_8);
	port_wr(PORT_COM1 + 4, 0x1e, PS_8);
	port_wr(PORT_COM1, 0xae, PS_8);
	
	if (port_rd(PORT_COM1, PS_8) != 0xae)
		return 1;
	
	port_wr(PORT_COM1 + 4, 0xf, PS_8);
	
	return 0;
}

char
sp_read_ch(void)
{
	while (!(port_rd(PORT_COM1 + 5, PS_8) & 0x1))
		;
	
	return port_rd(PORT_COM1, PS_8);
}

void
sp_write_ch(char ch)
{
	while (!(port_rd(PORT_COM1 + 5, PS_8) & 0x20))
		;
	
	port_wr(PORT_COM1, ch, PS_8);
}

void
sp_write_str(char const *s)
{
	for (char const *c = s; *c; ++c)
		sp_write_ch(*c);
}
