#include "mm/kheap.h"

#include "kutil.h"
#include "mm/pmm.h"

#define KH_START 0xfffffbbb00000000
#define KH_MAX_PAGES 65536

struct kh_ent {
	struct kh_ent *next;
	size_t data_size;
};

static struct kh_ent *head;
static size_t kh_pages;

int
kheap_init(void)
{
	ku_println(LT_INFO, "kheap: init");
	
	return 0;
}

void *
kheap_alloc(size_t n)
{
}

void
kheap_free(void *addr)
{
}
