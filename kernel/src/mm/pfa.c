#include "mm/pfa.h"

#include <stddef.h>
#include <stdint.h>

#include "kutil.h"
#include "mm/mlayt.h"

#define PAGE_STK_CAP 256

struct bitmap {
	paddr_t base;
	uint8_t *data;
	size_t npage, first_free;
};

static void bm_feed_stk(void);
static size_t bm_find_first_free(struct bitmap const *bm, size_t start);

static size_t page_bm_cnt;
static struct bitmap page_bms[MLAYT_MAX_ENTS];
static size_t page_stk_size;
static paddr_t page_stk[PAGE_STK_CAP];

void
pfa_init(void)
{
	ku_log(LT_INFO, "initializing page frame allocator");
	
	struct mlayt_ent const *ml_ents = mlayt_get(&page_bm_cnt);
	for (size_t i = 0; i < page_bm_cnt; ++i) {
		// this approach allows some memory to be unused, but it should
		// be negligible.
		
		size_t npage_total = ml_ents[i].size / PAGE_SIZE;
		size_t npage_bm = 1 + (7 * npage_total / 8 - 1) / PAGE_SIZE;
		
		page_bms[i] = (struct bitmap){
			.base = ml_ents[i].base + PAGE_SIZE * npage_bm,
			.data = (uint8_t *)ml_ents[i].base,
			.npage = npage_total - npage_bm,
		};
	}
	
	page_stk_size = 0;
	bm_feed_stk();
}

bool
pfa_avl(paddr_t addr)
{
	addr &= ~(PAGE_SIZE - 1);
	
	struct bitmap const *bm;
	for (size_t i = 0; i < page_bm_cnt; ++i) {
		bm = &page_bms[i];
		if (bm->base <= addr && addr < bm->base + PAGE_SIZE * bm->npage)
			goto found;
	}
	return false;
found:;
	size_t ind = (addr - bm->base) / PAGE_SIZE;
	size_t byte = ind / 8, bit = ind % 8;
	
	return !!(bm->data[byte] & 1 << bit);
}

paddr_t
pfa_alloc(void)
{
	if (!page_stk_size)
		bm_feed_stk();
	
	// if no pages could be fed to stack, just return `PADDR_NULL`.
	if (!page_stk_size)
		return PADDR_NULL;
	
	return page_stk[--page_stk_size];
}

void
pfa_free(paddr_t addr)
{
	// TODO: fix not working code.
	
	addr &= ~(PAGE_SIZE - 1);
	
	struct bitmap *bm;
	for (size_t i = 0; i < page_bm_cnt; ++i) {
		bm = &page_bms[i];
		if (bm->base <= addr && addr < bm->base + PAGE_SIZE * bm->npage)
			goto found;
	}
	return;
found:;
	size_t ind = (addr - bm->base) / PAGE_SIZE;
	size_t byte = ind / 8, bit = ind % 8;
	
	bm->data[byte] &= ~(1 << bit);
	bm->first_free = ind;
}

static void
bm_feed_stk(void)
{
	for (size_t i = 0; i < page_bm_cnt && page_stk_size < PAGE_STK_CAP; ++i) {
		struct bitmap *bm = &page_bms[i];
		
		for (size_t j = bm->first_free; j < bm->npage; ++j) {
			size_t byte = j / 8, bit = j % 8;
			if (!(bm->data[byte] & 1 << bit)) {
				bm->data[byte] |= 1 << bit;
				page_stk[page_stk_size++] = bm->base + PAGE_SIZE * j;
			}
			
			if (page_stk_size >= PAGE_STK_CAP)
				break;
		}
		
		bm->first_free = bm_find_first_free(bm, bm->first_free + 1);
	}
}

static size_t
bm_find_first_free(struct bitmap const *bm, size_t start)
{
	size_t free;
	
	for (free = start; free < bm->npage; ++free) {
		size_t byte = free / 8, bit = free % 8;
		if (!(bm->data[byte] & 1 << bit))
			break;
	}
	
	return free;
}