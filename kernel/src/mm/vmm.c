#include "mm/vmm.h"

#include <stddef.h>

#include <limine.h>

#include "kutil.h"
#include "mm/mem_layout.h"
#include "mm/pmm.h"
#include "sys/cpu.h"

#define K_STK_PAGES 4
#define K_STK_BASE 0xffffffff70000000

typedef uint64_t page_dir_t;

struct page_index {
	size_t pdpt, pdt, pt, pg;
};

extern void const volatile K_START_VADDR__;
extern void const volatile K_END_VADDR__;

static struct page_index vaddr_to_pg_ind(void const *vaddr);
static page_dir_t *next_pg_dir(page_dir_t *cur, size_t ind, uint8_t flags);
static page_dir_t *some_next_pg_dir(page_dir_t *cur, size_t ind);

static struct limine_kernel_address_request volatile k_addr_req = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0,
};

void
vmm_init(void (*post_init_jmp)(void))
{
	ku_log(LT_INFO, "initializing virtual memory manager and repaging kernel");
	
	phys_addr_t pml4 = vmm_mk_map();
	
	// map kernel.
	size_t k_size = (uintptr_t)&K_END_VADDR__ - (uintptr_t)&K_START_VADDR__;
	for (size_t i = 0; i < k_size; i += PAGE_SIZE) {
		phys_addr_t paddr = k_addr_req.response->physical_base + i;
		void *vaddr = (void *)(k_addr_req.response->virtual_base + i);
		vmm_map(pml4, paddr, vaddr, VF_RW);
	}
	
	// map usable memory regions.
	size_t ml_ent_cnt;
	struct meml_ent const *ml_ents = meml_get(&ml_ent_cnt);
	for (size_t i = 0; i < ml_ent_cnt; ++i) {
		for (size_t j = 0; j < ml_ents[i].size; j += PAGE_SIZE) {
			phys_addr_t paddr = ml_ents[i].base + j;
			void *vaddr = (void *)(ml_ents[i].base + j);
			vmm_map(pml4, paddr, vaddr, VF_RW);
		}
	}
	
	// create and map new kernel stack.
	for (unsigned i = 0; i < K_STK_PAGES; ++i) {
		phys_addr_t paddr = pmm_alloc();
		void *vaddr = (void *)(K_STK_BASE - PAGE_SIZE * (i + 1));
		vmm_map(pml4, paddr, vaddr, VF_RW);
	}
	
	cpu_switch_mem_ctx(pml4, (void *)K_STK_BASE, post_init_jmp);
}

phys_addr_t
vmm_mk_map(void)
{
	phys_addr_t pml4 = pmm_alloc();
	ku_fms_64((void *)pml4, 0, PAGE_SIZE / 64);
	
	// identity map first megabyte.
	// should eventually be replaced with more efficient loop using
	// `ku_fms_64()`, but good enough for now.
	for (phys_addr_t i = 0x0; i < 0x100000; i += PAGE_SIZE)
		vmm_map(pml4, i, (void *)i, VF_RW);
	
	return pml4;
}

void
vmm_map(phys_addr_t pml4, phys_addr_t paddr, void const *vaddr, uint8_t flags)
{
	paddr &= ~(PAGE_SIZE - 1);
	
	struct page_index pi = vaddr_to_pg_ind(vaddr);
	
	page_dir_t *pdpt = next_pg_dir((page_dir_t *)pml4, pi.pdpt, flags);
	page_dir_t *pdt = next_pg_dir(pdpt, pi.pdt, flags);
	page_dir_t *pt = next_pg_dir(pdt, pi.pt, flags);
	
	pt[pi.pg] = paddr;
	pt[pi.pg] |= VF_PRESENT | flags;
}

void
vmm_unmap(phys_addr_t pml4, void const *vaddr)
{
	struct page_index pi = vaddr_to_pg_ind(vaddr);
	
	page_dir_t *pdpt = some_next_pg_dir((page_dir_t *)pml4, pi.pdpt);
	page_dir_t *pdt = some_next_pg_dir(pdpt, pi.pdt);
	page_dir_t *pt = some_next_pg_dir(pdt, pi.pt);
	
	if (!pt)
		return;
	
	pmm_free(pt[pi.pg] & ~0xfff);
	pt[pi.pg] = 0;
}

void
vmm_map_cr3(phys_addr_t paddr, void const *vaddr, uint8_t flags)
{
	// TODO: cache `cr3` for map and unmap.
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	vmm_map(cr.cr3, paddr, vaddr, flags);
}

void
vmm_unmap_cr3(void const *vaddr)
{
	struct cpu_ctl_regs cr = cpu_get_ctl_regs();
	vmm_unmap(cr.cr3, vaddr);
}

static struct page_index
vaddr_to_pg_ind(void const *vaddr)
{
	return (struct page_index){
		.pdpt = (uintptr_t)vaddr >> 39 & 0x1ff,
		.pdt = (uintptr_t)vaddr >> 30 & 0x1ff,
		.pt = (uintptr_t)vaddr >> 21 & 0x1ff,
		.pg = (uintptr_t)vaddr >> 12 & 0x1ff,
	};
}

static page_dir_t *
next_pg_dir(page_dir_t *cur, size_t ind, uint8_t flags)
{
	if (!(cur[ind] & VF_PRESENT)) {
		phys_addr_t new = pmm_alloc();
		ku_fms_64((void *)new, 0, PAGE_SIZE / 8);
		
		cur[ind] = new;
		cur[ind] |= VF_PRESENT | flags;
	}
	
	return (page_dir_t *)(cur[ind] & ~0xfff);
}

static page_dir_t *
some_next_pg_dir(page_dir_t *cur, size_t ind)
{
	if (!cur || !(cur[ind] & VF_PRESENT))
		return NULL;
	
	return (page_dir_t *)(cur[ind] & ~0xfff);
}
