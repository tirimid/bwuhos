#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#include "kdef.h"

#if defined(K_ARCH_X86_64)
typedef phys_addr_t page_map_t;
#endif

enum vmm_flag {
	VF_WRITE = 0x1,
	VF_NO_EXEC = 0x2,
	VF_NO_CACHE = 0x4,
};

void vmm_init(void (*post_init_jmp)(void));
page_map_t vmm_create_map(void);
void vmm_destroy_map(page_map_t map);
void vmm_map(page_map_t map, phys_addr_t paddr, void const *vaddr, uint8_t flags);
void vmm_unmap(page_map_t map, void const *vaddr);
void vmm_map_cur(phys_addr_t paddr, void const *vaddr, uint8_t flags);
void vmm_unmap_cur(void const *vaddr);

#endif
