#include "vmm.h"

#include <limine.h>

#include "kdef.h"
#include "kutil.h"
#include "mem_layout.h"
#include "pmm.h"

#if defined(K_ARCH_X86_64)
#include "arch/cpu.h"
#include "arch/paging.h"
#endif

#define K_STK_PAGES 8
#define K_STK_BASE 0xffffffff40000000

extern void const volatile K_START_VADDR;
extern void const volatile K_END_VADDR;

static struct limine_kernel_address_request volatile k_addr_req = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0,
};

void
vmm_init(void (*post_init_jmp)(void))
{
	ku_println(LT_INFO, "vmm: init and repaging kernel");
	
	page_map_t map = vmm_create_map();
	
	// map kernel.
	size_t k_size = (uintptr_t)&K_END_VADDR - (uintptr_t)&K_START_VADDR;
	for (size_t i = 0; i < k_size; i += PAGE_SIZE) {
		phys_addr_t paddr = k_addr_req.response->physical_base + i;
		void *vaddr = (void *)(k_addr_req.response->virtual_base + i);
		vmm_map(map, paddr, vaddr, VF_WRITE);
	}
	
	// map usable memory regions.
	size_t ml_ent_cnt;
	struct meml_ent const *ml_ents = meml_get(&ml_ent_cnt);
	for (size_t i = 0; i < ml_ent_cnt; ++i) {
		for (size_t j = 0; j < ml_ents[i].size; j += PAGE_SIZE) {
			phys_addr_t paddr = ml_ents[i].base + j;
			void *vaddr = (void *)(ml_ents[i].base + j);
			vmm_map(map, paddr, vaddr, VF_WRITE);
		}
	}
	
	// create and map new kernel stack.
	for (unsigned i = 0; i < K_STK_PAGES; ++i) {
		phys_addr_t paddr = pmm_alloc();
		void *vaddr = (void *)(K_STK_BASE + PAGE_SIZE * i);
		vmm_map(map, paddr, vaddr, VF_WRITE);
	}
	
#if defined(K_ARCH_X86_64)
	uintptr_t stk_base = K_STK_BASE + PAGE_SIZE * K_STK_PAGES;
	cpu_switch_mem_ctx(map, (void *)stk_base, post_init_jmp);
#endif
}

page_map_t
vmm_create_map(void)
{
#if defined(K_ARCH_X86_64)
	return paging_create_map();
#endif
}

void
vmm_destroy_map(page_map_t map)
{
#if defined(K_ARCH_X86_64)
	paging_destroy_map(map);
#endif
}

void
vmm_map(page_map_t map, phys_addr_t paddr, void const *vaddr, uint8_t flags)
{
#if defined(K_ARCH_X86_64)
	uint64_t x86_flags = PF_P;
	x86_flags |= !!(flags & VF_WRITE) * PF_RW;
	x86_flags |= !!(flags & VF_NO_EXEC) * PF_XD;
	x86_flags |= !!(flags & VF_NO_CACHE) * PF_PCD;
	
	paging_map(map, paddr, vaddr, x86_flags);
#endif
}

void
vmm_unmap(page_map_t map, void const *vaddr)
{
#if defined(K_ARCH_X86_64)
	paging_unmap(map, vaddr);
#endif
}

void
vmm_map_cur(phys_addr_t paddr, void const *vaddr, uint8_t flags)
{
#if defined(K_ARCH_X86_64)
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	vmm_map(cr.cr3, paddr, vaddr, flags);
	paging_invlpg(vaddr);
#endif
}

void
vmm_unmap_cur(void const *vaddr)
{
#if defined(K_ARCH_X86_64)
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	vmm_unmap(cr.cr3, vaddr);
	paging_invlpg(vaddr);
#endif
}
