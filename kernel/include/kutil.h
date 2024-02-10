#ifndef KUTIL_H__
#define KUTIL_H__

#include <stddef.h>
#include <stdint.h>

enum log_type {
	LT_INFO = 0,
	LT_WARN,
	LT_ERR,
	LT_DEBUG,
};

void ku_hang(void);
void ku_log(enum log_type type, char const *msg, ...);

// fast memset.
void ku_fms_8(void *dst, uint8_t b, size_t n);
void ku_fms_16(void *dst, uint8_t w, size_t n);
void ku_fms_32(void *dst, uint8_t d, size_t n);
void ku_fms_64(void *dst, uint8_t q, size_t n);

// slow memcpy.
void ku_smc_8(void *dst, void const *src, size_t n);

#endif
