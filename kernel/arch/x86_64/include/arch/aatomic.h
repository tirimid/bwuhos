#ifndef ARCH_AATOMIC_H
#define ARCH_AATOMIC_H

#include <stdint.h>

void aatomic_set(uint8_t *p);
void aatomic_clr(uint8_t *p);

#endif
