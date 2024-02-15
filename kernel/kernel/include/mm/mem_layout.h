#ifndef MM_MEM_LAYOUT_H__
#define MM_MEM_LAYOUT_H__

#include <stddef.h>

#include "kdef.h"

#define MEML_MAX_ENTS 32

struct meml_ent {
	phys_addr_t base;
	size_t size;
};

// only usable entries are collected in `meml_init()`.
// TODO: add memory reclaiming from bootloader during init stage 2.
int meml_init(void);
struct meml_ent const *meml_get(size_t *out_size);

#endif
