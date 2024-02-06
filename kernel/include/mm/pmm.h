#ifndef MM_PMM_H__
#define MM_PMM_H__

#include <stdbool.h>

#include "kdef.h"

void pmm_init(void);
bool pmm_avl(phys_addr_t addr);
phys_addr_t pmm_alloc(void);
void pmm_free(phys_addr_t addr);

#endif
