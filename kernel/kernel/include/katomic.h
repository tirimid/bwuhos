#ifndef KATOMIC_H
#define KATOMIC_H

#include <stdint.h>

#if defined(K_ARCH_X86_64)
typedef uint8_t k_mutex_t;
#endif

void k_mutex_lock(k_mutex_t *mutex);
void k_mutex_unlock(k_mutex_t *mutex);

#endif
