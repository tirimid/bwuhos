#ifndef MM_PFA_H__
#define MM_PFA_H__

#include <stdbool.h>

#include "kdef.h"

void pfa_init(void);
bool pfa_avl(paddr_t addr);
paddr_t pfa_alloc(void);
paddr_t pfa_zalloc(void);
void pfa_free(paddr_t addr);

#endif
