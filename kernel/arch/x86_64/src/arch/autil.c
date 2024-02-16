#include "arch/autil.h"

#include <stdarg.h>

#include "dev/serial_port.h"

void
au_println(enum log_type type, char const *msg, ...)
{
	va_list args;
	va_start(args, msg);
	ku_print(type, "x86_64: ");
	ku_print_v(LT_NONE, msg, args);
	sp_write_ch('\n');
	va_end(args);
}
