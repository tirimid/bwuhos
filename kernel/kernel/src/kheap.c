#include "kheap.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "katomic.h"
#include "kdef.h"
#include "kutil.h"
#include "pmm.h"
#include "vmm.h"

#define KH_START 0xfffffbbb00000000
#define KH_MAX_NPAGES 65536

struct kh_ent {
	struct kh_ent *next;
	size_t data_size;
};

static bool is_free(void const *base, size_t limit);
static void *find_free(size_t n);

static struct kh_ent *kh_head, *kh_tail;
static size_t kh_npages;
static k_mutex_t mutex;

int
kheap_init(void)
{
	ku_println(LT_INFO, "kheap: init");
	
	phys_addr_t paddr = pmm_alloc();
	if (!paddr) {
		ku_println(LT_ERR, "kheap: failed to alloc page for initial heap!");
		return 1;
	}
	vmm_map_cur(paddr, (void *)KH_START, VF_WRITE);
	kh_npages = 1;
	
	// `head` is just a dummy entry that will not actually store any data.
	// otherwise heap could only be initialized upon first allocation, which
	// I dislike architecturally.
	kh_tail = kh_head = (void *)KH_START;
	*kh_head = (struct kh_ent){
		.next = NULL,
		.data_size = 0,
	};
	
	return 0;
}

void *
kheap_alloc(size_t n)
{
	k_mutex_lock(&mutex);
	
	struct kh_ent *new = find_free(n);
	if (new) {
		*new = (struct kh_ent){
			.next = NULL,
			.data_size = n,
		};
		
		kh_tail->next = new;
		kh_tail = new;
		
		k_mutex_unlock(&mutex);
		return (char *)new + sizeof(struct kh_ent);
	}
	
	// maybe the data is too large and spans multiple pages.
	// try, acknowledging this possibility, until no more pages are allowed
	// to be allocated.
	while (kh_npages < KH_MAX_NPAGES) {
		phys_addr_t paddr = pmm_alloc();
		if (!paddr) {
			ku_println(LT_ERR, "kheap: failed to alloc page for heap expansion!");
			k_mutex_unlock(&mutex);
			return NULL;
		}
		vmm_map_cur(paddr, (void *)(KH_START + PAGE_SIZE * kh_npages), VF_WRITE);
		++kh_npages;
		
		struct kh_ent *new = find_free(n);
		if (new) {
			*new = (struct kh_ent){
				.next = NULL,
				.data_size = n,
			};
			
			kh_tail->next = new;
			kh_tail = new;
			
			k_mutex_unlock(&mutex);
			return (char *)new + sizeof(struct kh_ent);
		}
	}
	
	k_mutex_unlock(&mutex);
	return NULL;
}

void
kheap_free(void *addr)
{
	// TODO: implement heap free.
}

static bool
is_free(void const *base, size_t limit)
{
	for (struct kh_ent const *ent = kh_head; ent; ent = ent->next) {
		uintptr_t base_u = (uintptr_t)base, ent_u = (uintptr_t)ent;
		
		if (base_u + limit > KH_START + PAGE_SIZE * kh_npages)
			return false;
		
		if (base_u < ent_u + sizeof(struct kh_ent) + ent->data_size
		    && base_u + limit > ent_u) {
			return false;
		}
	}
	
	return true;
}

static void *
find_free(size_t n)
{
	for (struct kh_ent *ent = kh_head; ent; ent = ent->next) {
		// `addr` is `sizeof(struct kh_ent)`-aligned.
		uintptr_t addr = (uintptr_t)ent;
		addr += sizeof(struct kh_ent) + ent->data_size;
		addr += sizeof(struct kh_ent) - addr % sizeof(struct kh_ent);
		
		if (is_free((void *)addr, sizeof(struct kh_ent) + n))
			return (void *)addr;
	}
	
	return NULL;
}
