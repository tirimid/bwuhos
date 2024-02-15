#include "dev/serial_port.h"

#include "kdef.h"
#include "sys/port.h"

int
sp_init(void)
{
	// check OSDev wiki article "Serial Ports" for init explanation.
	port_wr_8(P_COM1 + 1, 0x0);
	port_wr_8(P_COM1 + 3, 0x80);
	port_wr_8(P_COM1, 0x3);
	port_wr_8(P_COM1 + 1, 0x0);
	port_wr_8(P_COM1 + 3, 0x3);
	port_wr_8(P_COM1 + 2, 0xc7);
	port_wr_8(P_COM1 + 4, 0xb);
	port_wr_8(P_COM1 + 4, 0x1e);
	port_wr_8(P_COM1, 0xae);
	
	if (port_rd_8(P_COM1) != 0xae)
		return 1;
	
	port_wr_8(P_COM1 + 4, 0xf);
	
	return 0;
}

char
sp_read_ch(void)
{
	while (!(port_rd_8(P_COM1 + 5) & 0x1))
		;
	
	return port_rd_8(P_COM1);
}

void
sp_write_ch(char ch)
{
	while (!(port_rd_8(P_COM1 + 5) & 0x20))
		;
	
	port_wr_8(P_COM1, ch);
}

void
sp_write_str(char const *s)
{
	for (char const *c = s; *c; ++c)
		sp_write_ch(*c);
}
