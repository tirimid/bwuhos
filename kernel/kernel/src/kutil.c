#include "kutil.h"

#include "dev/serial_port.h"
#include "katomic.h"

#if defined(K_ARCH_X86_64)
#include "arch/autil.h"
#endif

enum fmt_mod {
	FM_QWORD = 0x1,
	FM_DWORD = 0x2,
	FM_WORD = 0x4,
	FM_BYTE = 0x8,
};

static void print_fmt_x(uint64_t n, uint32_t mod);
static void print_fmt_b(uint64_t n, uint32_t mod);
static void print_fmt_u(uint64_t n, uint32_t mod);

void
ku_hang(void)
{
#if defined(K_ARCH_X86_64)
	au_hang();
#endif
}

void
ku_spin_cycles(unsigned long long ncycles)
{
	// assume, based on basically nothing, that every iteration will take
	// somewhere around 0.5 - 3 clock cycles.
	for (unsigned volatile i = 0; i < ncycles; ++i)
		;
}

void
ku_print(enum log_type type, char const *msg, ...)
{
	va_list args;
	va_start(args, msg);
	ku_print_v(type, msg, args);
	va_end(args);
}

void
ku_println(enum log_type type, char const *msg, ...)
{
	va_list args;
	va_start(args, msg);
	ku_print_v(type, msg, args);
	sp_write_ch('\n');
	va_end(args);
}

void
ku_print_v(enum log_type type, char const *msg, va_list args)
{
	static k_mutex_t mutex;
	k_mutex_lock(&mutex);
	
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
			print_fmt_x(va_arg(args, uint64_t), mod);
			break;
		case 'b':
			print_fmt_b(va_arg(args, uint64_t), mod);
			break;
		case 'u':
			print_fmt_u(va_arg(args, uint64_t), mod);
			break;
		default:
			break;
		}
	}
	
	k_mutex_unlock(&mutex);
}

void
ku_memset(void *dst, uint8_t b, size_t n)
{
#if defined(K_ARCH_X86_64)
	uint64_t q = b;
	q |= (uint64_t)b << 8;
	q |= (uint64_t)b << 16;
	q |= (uint64_t)b << 24;
	q |= (uint64_t)b << 32;
	q |= (uint64_t)b << 40;
	q |= (uint64_t)b << 48;
	q |= (uint64_t)b << 56;
	
	au_fms_64(dst, q, n / 8);
	for (size_t i = n - n % 3; i < n; ++i)
		*((uint8_t *)dst + i) = b;
#endif
}

void
ku_memcpy(void *dst, void const *src, size_t n)
{
	// TODO: CPU efficient implementation using string operations.
	for (size_t i = 0; i < n; ++i)
		*((uint8_t *)dst + i) = *((uint8_t *)src + i);
}

static void
print_fmt_x(uint64_t n, uint32_t mod)
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
print_fmt_b(uint64_t n, uint32_t mod)
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
print_fmt_u(uint64_t n, uint32_t mod)
{
	// TODO: implement unsigned decimal formatting.
}
