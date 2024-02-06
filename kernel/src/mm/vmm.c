#include "mm/vmm.h"

#include <stddef.h>

#include "kutil.h"
#include "mm/pmm.h"
#include "sys/cpu.h"

typedef uint64_t page_dir_t;

struct page_index {
	size_t pdpt, pdt, pt, pg;
};

static struct page_index vaddr_to_pg_ind(void const *vaddr);
static page_dir_t *next_pg_dir(page_dir_t *cur, size_t ind, uint8_t flags);
static page_dir_t *some_next_pg_dir(page_dir_t *cur, size_t ind);

phys_addr_t
vmm_mk_map(void)
{
	phys_addr_t pml4 = pmm_alloc();
	ku_fms_64((void *)pml4, 0, PAGE_SIZE / 64);
	
	// identity map first megabyte.
	// should eventually be replaced with more efficient loop using
	// `ku_fms_64()`, but good enough for now.
	for (phys_addr_t i = 0x0; i < 0x100000; i += PAGE_SIZE)
		vmm_map(pml4, i, (void *)i, VF_PRESENT | VF_RW);
	
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
	vmm_invlpg(vaddr);
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
	vmm_invlpg(vaddr);
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
