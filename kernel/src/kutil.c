#include "kutil.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "dev/serial_port.h"

enum fmt_mod {
	FM_QWORD = 0x1,
	FM_DWORD = 0x2,
	FM_WORD = 0x4,
	FM_BYTE = 0x8,
};

static void fmt_log_x(uint64_t n, uint32_t mod);
static void fmt_log_b(uint64_t n, uint32_t mod);
static void fmt_log_u(uint64_t n, uint32_t mod);

void
ku_log(enum log_type type, char const *msg, ...)
{
	va_list args;
	va_start(args, msg);
	
	switch (type) {
	case LT_NONE:
		break;
	case LT_INFO:
		sp_write_str("[info] ");
		break;
	case LT_WARN:
		sp_write_str("[warn] ");
		break;
	case LT_ERR:
		sp_write_str("[err] ");
		break;
	case LT_DEBUG:
		sp_write_str("[debug] ");
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
		
		uint32_t mod = 0;
		switch (*(c + 1)) {
		case 'Q':
			mod |= FM_QWORD;
			++c;
			break;
		case 'D':
			mod |= FM_DWORD;
			++c;
			break;
		case 'W':
			mod |= FM_WORD;
			++c;
			break;
		case 'B':
			mod |= FM_BYTE;
			++c;
			break;
		default:
			break;
		}
		
		switch (*(c++ + 1)) {
		case 'x':
			fmt_log_x(va_arg(args, uint64_t), mod);
			break;
		case 'b':
			fmt_log_b(va_arg(args, uint64_t), mod);
			break;
		case 'u':
			fmt_log_u(va_arg(args, uint64_t), mod);
			break;
		default:
			break;
		}
	}
	
	sp_write_ch('\n');
	
	va_end(args);
}

void
ku_smc_8(void *dst, void const *src, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		*((uint8_t *)dst + i) = *((uint8_t *)src + i);
}

static void
fmt_log_x(uint64_t n, uint32_t mod)
{
	static char const hex_lut[] = "0123456789abcdef";
	
	size_t nch;
	if (mod & FM_QWORD)
		nch = 16;
	else if (mod & FM_DWORD)
		nch = 8;
	else if (mod & FM_WORD)
		nch = 4;
	else if (mod & FM_BYTE)
		nch = 2;
	else {
		if (!n) {
			sp_write_ch('0');
			return;
		}
		
		nch = 0;
		while (nch < 16 && n >> 4 * nch)
			++nch;
	}
	
	char text[17] = {0};
	for (size_t i = 0; i < nch; ++i)
		text[nch - i - 1] = hex_lut[n >> 4 * i & 0xf];
	
	sp_write_str(text);
}

static void
fmt_log_b(uint64_t n, uint32_t mod)
{
	size_t nch;
	if (mod & FM_QWORD)
		nch = 64;
	else if (mod & FM_DWORD)
		nch = 32;
	else if (mod & FM_WORD)
		nch = 16;
	else if (mod & FM_BYTE)
		nch = 8;
	else {
		if (!n) {
			sp_write_ch('0');
			return;
		}
		
		nch = 0;
		while (nch < 64 && n >> nch)
			++nch;
	}
	
	char text[65] = {0};
	for (size_t i = 0; i < nch; ++i)
		text[nch - i - 1] = n >> i & 0x1 ? '1' : '0';
	
	sp_write_str(text);
}

static void
fmt_log_u(uint64_t n, uint32_t mod)
{
	// TODO: implement unsigned decimal formatting.
}
