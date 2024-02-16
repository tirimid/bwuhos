#ifndef ARCH_AUTIL_H
#define ARCH_AUTIL_H

#include <stddef.h>
#include <stdint.h>

#include "kutil.h"

void au_hang(void);
void au_println(enum log_type type, char const *msg, ...);

// fast memset using string operations.
void au_fms_64(void *dst, uint64_t q, size_t n);

#endif
