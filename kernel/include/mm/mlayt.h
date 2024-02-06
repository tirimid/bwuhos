#ifndef MM_MLAYT_H__
#define MM_MLAYT_H__

#include <stddef.h>

#include "kdef.h"

struct mlayt_ent {
	paddr_t base;
	size_t size;
};

// only usable entries are collected in `mlayt_init()`.
int mlayt_init(void);
struct mlayt_ent const *mlayt_get(size_t *out_size);

#endif
