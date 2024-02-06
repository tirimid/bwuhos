#include "mm/mlayt.h"

#include "limine.h"

#include "kdef.h"
#include "kutil.h"

static struct mlayt_ent ml_ents[MLAYT_MAX_ENTS];
static size_t ml_size;

static struct limine_memmap_request volatile mem_map_req = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
};

int
mlayt_init(void)
{
	ku_log(LT_INFO, "initializing memory layout");
	
	ml_size = 0;
	size_t resp_ent_cnt = mem_map_req.response->entry_count;
	
	for (size_t i = 0; i < resp_ent_cnt && ml_size < MLAYT_MAX_ENTS; ++i) {
		struct limine_memmap_entry const *lmn_ent = mem_map_req.response->entries[i];
		
		if (lmn_ent->type != LIMINE_MEMMAP_USABLE
		    || lmn_ent->length < 0x10 * PAGE_SIZE) {
			continue;
		}
		
		ml_ents[ml_size++] = (struct mlayt_ent){
			.base = lmn_ent->base,
			.size = lmn_ent->length,
		};
	}
	
	if (!ml_size) {
		ku_log(LT_ERR, "no usable memory layout entries found");
		return 1;
	}
	
	return 0;
}

struct mlayt_ent const *
mlayt_get(size_t *out_size)
{
	if (out_size)
		*out_size = ml_size;
	
	return ml_ents;
}
