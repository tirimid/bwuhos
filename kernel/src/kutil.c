#include "kutil.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "dev/serial_port.h"

static void fmt_log_x(uint64_t n);
static void fmt_log_b(uint64_t n);

void
ku_log(enum log_type type, char const *msg, ...)
{
	va_list args;
	va_start(args, msg);
	
	switch (type) {
	case LT_INFO:
		sp_write_str("[info] ");
		break;
	case LT_DEBUG:
		sp_write_str("[debug] ");
		break;
	case LT_ERR:
		sp_write_str("[err] ");
		break;
	}
	
	for (char const *c = msg; *c; ++c) {
		if (*c != '%') {
			sp_write_ch(*c);
			continue;
		}
		
		if (!*(c + 1) || *(c + 1) == '%') {
			sp_write_ch(*c);
			++c;
			continue;
		}
		
		switch (*(c++ + 1)) {
		case 'x':
			fmt_log_x(va_arg(args, uint64_t));
			break;
		case 'b':
			fmt_log_b(va_arg(args, uint64_t));
			break;
		default:
			break;
		}
	}
	
	sp_write_ch('\n');
	
	va_end(args);
}

static void
fmt_log_x(uint64_t n)
{
	static char const hex_lut[] = "0123456789abcdef";
	
	if (!n) {
		sp_write_ch('0');
		return;
	}
	
	size_t nch = 0;
	while (nch < 16 && n >> 4 * nch)
		++nch;
	
	char text[17] = {0};
	for (size_t i = 0; i < nch; ++i)
		text[nch - i - 1] = hex_lut[n >> 4 * i & 0xf];
	
	sp_write_str(text);
}

static void
fmt_log_b(uint64_t n)
{
	if (!n) {
		sp_write_ch('0');
		return;
	}
	
	size_t nch = 0;
	while (nch < 64 && n >> nch)
		++nch;
	
	char text[65] = {0};
	for (size_t i = 0; i < nch; ++i)
		text[nch - i - 1] = n >> i & 0x1 ? '1' : '0';
	
	sp_write_str(text);
}
