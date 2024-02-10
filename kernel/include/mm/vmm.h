#ifndef MM_VMM_H__
#define MM_VMM_H__

#include <stdint.h>

#include "kdef.h"

enum vmm_flag {
	VF_PRESENT = 0x1,
	VF_RW = 0x2,
	VF_USER_SUPER = 0x4,
	VF_WRITE_THRU = 0x8,
	VF_NO_CACHE = 0x10,
	VF_ACCESSED = 0x20,
	VF_SIZE = 0x40,
};

void vmm_init(void (*post_init_jmp)(void));
phys_addr_t vmm_mk_map(void);
void vmm_map(phys_addr_t pml4, phys_addr_t paddr, void const *vaddr, uint8_t flags);
void vmm_unmap(phys_addr_t pml4, void const *vaddr);
void vmm_map_cr3(phys_addr_t paddr, void const *vaddr, uint8_t flags);
void vmm_unmap_cr3(void const *vaddr);
void vmm_invlpg(void const *vaddr);

#endif
