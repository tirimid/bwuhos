#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>

int kheap_init(void);
void *kheap_alloc(size_t n);
void kheap_free(void *addr);

#endif
