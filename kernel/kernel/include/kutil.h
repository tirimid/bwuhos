#ifndef KUTIL_H__
#define KUTIL_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

enum log_type {
	LT_NONE = 0,
	LT_INFO,
	LT_WARN,
	LT_ERR,
	LT_DEBUG,
};

void ku_hang(void);
void ku_spin_cycles(unsigned long long ncycles);
void ku_print(enum log_type type, char const *msg, ...);
void ku_println(enum log_type type, char const *msg, ...);
void ku_print_v(enum log_type type, char const *msg, va_list args);
void ku_memset(void *dst, uint8_t b, size_t n);
void ku_memcpy(void *dst, void const *src, size_t n);

#endif
