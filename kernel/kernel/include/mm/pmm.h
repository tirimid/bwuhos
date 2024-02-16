#ifndef MM_PMM_H
#define MM_PMM_H

#include <stdbool.h>

#include "kdef.h"

void pmm_init(void);
bool pmm_avl(phys_addr_t addr);
phys_addr_t pmm_alloc(void);
void pmm_free(phys_addr_t addr);

#endif
