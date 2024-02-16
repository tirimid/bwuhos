#ifndef ARCH_PAGING_H__
#define ARCH_PAGING_H__

#include <stdint.h>

#include "kdef.h"

enum paging_flag {
	PF_P = 0x1,
	PF_RW = 0x2,
	PF_US = 0x4,
	PF_PWT = 0x8,
	PF_PCD = 0x10,
	PF_A = 0x20,
	
	// PS and dependent flags omitted.
};

phys_addr_t paging_mk_map(void);
void paging_map(phys_addr_t pml4, phys_addr_t paddr, void const *vaddr, uint8_t flags);
void paging_unmap(phys_addr_t pml4, void const *vaddr);
void paging_invlpg(void const *vaddr);

#endif
